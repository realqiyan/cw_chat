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

void initNetwork();
void processNetwork();

void callback(char* topic, unsigned char* payload, unsigned int length);
void reconnect();
void setupWifi();
void processMsg(String msg);
void sendMessage();
const MorseInput convertInput(String inputStr);

WiFiClient espClient;
PubSubClient client(espClient);

void initNetwork() {
  setupWifi();
  client.setServer(cwConfig.mqttServer.c_str(), cwConfig.mqttPort);
  client.setBufferSize(5120);
  client.setCallback(callback);
}

void processNetwork() {
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

void setupWifi() {
  changeNetworkStatus(0);
  delay(20);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(String(cwConfig.ssid.c_str()));
  WiFi.begin(cwConfig.ssid.c_str(), cwConfig.password.c_str());
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (++count > 6) {
      WiFi.disconnect();
      changeNetworkStatus(-1);
      return;
    }
  }
  changeNetworkStatus(1);
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
    String clientId = "ESP8266Client-" + String(cwConfig.callsign.c_str());
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(cwConfig.topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

void sendMessage(list<MorseInput> inputs) {
  if (inputs.size() > 0) {
    string message;
    message = cwConfig.callsign.c_str();
    message = message + ":" + MorseInput::convert(inputs);
    char sendMessage[message.length()];
    strcpy(sendMessage, message.data());
    bool publishRet = client.publish(cwConfig.topic.c_str(), sendMessage);
  }
}

void processMsg(String msg) {
  int index = msg.indexOf(":");
  if (index < 0) {
    return;
  }
  // 消息处理
  String sender = msg.substring(0, index);
  // 小标题显示发送者
  updateSubTitle('#' + sender);
  if (String(cwConfig.callsign.c_str()) == sender) {
    return;
  }
  String message = msg.substring(index + 1);
  list<MorseInput> msgList = MorseInput::convert(message.c_str());
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
        char letterChar = MorseCode::getLetter(inputMorseCode.c_str());
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