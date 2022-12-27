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
  changeNetworkStatus(0);
  delay(20);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(cwConfig.ssid);
  WiFi.begin(cwConfig.ssid, cwConfig.password);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
    if (++count > 6) {
      Serial.println("WiFi connect fail");
      WiFi.disconnect();
      changeNetworkStatus(-1);
      return;
    }
  }
  changeNetworkStatus(1);
  Serial.println("WiFi connected");
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
    String clientId = "ESP8266Client-" + String(cwConfig.callsign);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(cwConfig.cwTopic.c_str());
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
  client.setServer(cwConfig.mqttServer.c_str(), cwConfig.mqttPort);
  client.setBufferSize(5120);
  client.setCallback(callback);
}

void checkNetwork() {
  //没有链接wifi直接退出
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  if (!client.connected()) {
    changeNetworkStatus(-2);
    reconnect();
  } else {
    changeNetworkStatus(2);
  }
  client.loop();
}

void sendMessage() {
  if (inputs.size() > 0) {
    string message;
    message = cwConfig.callsign.c_str();
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
    bool publishRet = client.publish(cwConfig.cwTopic.c_str(), sendMessage);
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
  String sender = msg.substring(0, index);
  if (String(cwConfig.callsign) == sender) {
    Serial.print("self:");
    Serial.println(sender);
    return;
  }

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
    displayLine(SHOW_CODE_LINE, String(it->input));
    playMorseInput(&*it);
    char inputChar = (*it).input;
    if (inputChar == ' ') {
      // 连续两个嘀嗒空格就输出一个空格（字母间隔会有一个空格，延时过长也会有一个空格）
      String displayLetter;
      if (inputMorseCode == "") {
        displayLetter = " ";
      } else {
        char letterChar = getLetter(inputMorseCode);
        displayLetter = String(letterChar);
        inputMorseCode = "";
      }
      displayLine(SHOW_LETTER_LINE, displayLetter);
    } else {
      inputMorseCode += inputChar;
    }
  }
}
#endif