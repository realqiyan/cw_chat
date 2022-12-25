#ifndef _CW_NETWORK_H
#define _CW_NETWORK_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string>
#include "cw_bee.h"
#include "cw_display.h"
#include "morse_code.h"
#include "morse_input.h"

using namespace std;

void sendMessage();
void setupWifi();
void processMsg(String msg);
const MorseInput convertInput(String inputStr);

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
  delay(20);
  Serial.print("Connecting to ");
  displayLine(1, "Wifi connection.");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  displayLine(1, "WiFi connected. ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, unsigned char* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
  processMsg(msg);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT connection...");
    displayLine(1, "MQTT connection.");
    String clientId = "ESP8266Client-" + callsign;
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      displayLine(1, "MQTT connected. ");
      client.subscribe(cwTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

void initNetwork() {
  setupWifi();
  client.setServer(mqttServer, mqttPort);
  client.setBufferSize(5120);
  client.setCallback(callback);
}

void checkNetwork() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void sendMessage() {
  if (inputs.size() > 0) {
    string message;
    message = callsign.c_str();
    message = message + ":";
    //使用迭代器输出list容器中的元素
    for (list<MorseInput>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
      message = message + (*it).input + "|" + to_string((*it).cost) + "|" + to_string((*it).span) + ";";
    }
    char sendMessage[message.length()];
    strcpy(sendMessage, message.data());
    Serial.print("publish message length:");
    Serial.println(message.length());
    Serial.println(sendMessage);
    bool publishRet = client.publish(cwTopic, sendMessage);
    Serial.print("publish result:");
    Serial.println(publishRet);
    inputs.clear();
  }
}

const MorseInput convertInput(String inputStr) {
  //input|cost|span
  //-|237|459
  int lastIdx = inputStr.lastIndexOf("|");
  char input = inputStr.substring(0, 1)[0];
  String cost = inputStr.substring(2, lastIdx);
  String span = inputStr.substring(lastIdx + 1);
  const MorseInput morseInput = { input, cost.toInt(), span.toInt() };
  return morseInput;
}

void processMsg(String msg) {
  int index = msg.indexOf(":");
  if (index < 0) {
    return;
  }
  // 消息处理
  list<MorseInput> msgList;
  String message = msg.substring(index + 1);
  while (message.indexOf(";") != -1) {
    int idx = message.indexOf(";");
    String inputStr = message.substring(0, idx);
    //解析输入对象
    const MorseInput morseInput = convertInput(inputStr);
    msgList.push_back(morseInput);
    message = message.substring(idx + 1);
  }

  //处理字符
  String inputMorseCode;
  for (list<MorseInput>::iterator it = msgList.begin(); it != msgList.end(); ++it) {
    playMorseInput(&*it);
    char inputChar = (*it).input;
    if (inputChar == ' ') {
      // 连续两个嘀嗒空格就输出一个空格（字母间隔会有一个空格，延时过长也会有一个空格）
      if (inputMorseCode == "") {
        displayLine(1, " ");
      } else {
        char letterChar = getLetter(inputMorseCode);
        String letter = String(letterChar);
        displayLine(1, letter);
        inputMorseCode = "";
      }
    } else {
      inputMorseCode += inputChar;
    }
  }
}
#endif