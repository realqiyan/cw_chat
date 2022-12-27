#ifndef _CW_CONFIG_H
#define _CW_CONFIG_H

#include <EEPROM.h>
#include <string>
#include <cstring>

using namespace std;

void initConfig();
bool checkConfig();
void saveCwConfig();
void readCwConfig();

void updateSsid(String ssid);
void updatePassword(String password);
void updateCallsign(String callsign);

void writeString(int* addr, String str);
String readString(int* addr);
void writeInt(int* addr, int val);
int readInt(int* addr);

struct CwConfig {
  //mqtt服务器端口
  int mqttPort;
  //mqtt服务器
  String mqttServer;
  //topic(聊天室)
  String cwTopic;
  //呼号
  String callsign;
  //WIFI名字
  String ssid;
  //WIFI密码
  String password;
  //配置版本（用于刷新EEPROM存储）
  long version;
};

CwConfig cwConfig = {
  1883, "broker-cn.emqx.io", "/cw/bg5aun", "BG5AUN", "***ssid***", "***password***", 1
};

//int|int|char[]|int|char[]|int|char[]|int|char[]|int|char[]
int configSize = 256;

bool checkVersion() {
  int versionSize = sizeof(cwConfig.version);
  int addr = configSize - versionSize;
  int romVersion = readInt(&addr);
  Serial.print("EEPROM data version:");
  Serial.println(romVersion);
  Serial.print("CwConfig data version:");
  Serial.println(cwConfig.version);
  return romVersion >= cwConfig.version;
}

void initConfig() {
  EEPROM.begin(configSize);
  if (checkVersion()) {
    readCwConfig();
  } else {
    saveCwConfig();
  }
}

void saveCwConfig() {
  Serial.println("save CwConfig");
  int addr = 0;
  //写入mqttPort
  writeInt(&addr, cwConfig.mqttPort);
  //写入服务器配置
  writeString(&addr, cwConfig.mqttServer);
  //写入topic
  writeString(&addr, cwConfig.cwTopic);
  //写入callsign
  writeString(&addr, cwConfig.callsign);
  //写入ssid
  writeString(&addr, cwConfig.ssid);
  //写入password
  writeString(&addr, cwConfig.password);

  Serial.print("save ssid:");
  Serial.println(cwConfig.ssid);

  // 存储标识
  int versionSize = sizeof(cwConfig.version);
  if (addr >= (configSize - versionSize)) {
    Serial.println("Overload! EEPROM commit failed");
    return;
  }
  addr = configSize - versionSize;
  writeInt(&addr, cwConfig.version);
  //提交数据
  if (EEPROM.commit()) {
    Serial.println("EEPROM successfully committed");
  } else {
    Serial.println("ERROR! EEPROM commit failed");
  }
}
void readCwConfig() {
  Serial.println("read CwConfig");
  int addr = 0;
  int mqttPort = readInt(&addr);
  cwConfig.mqttPort = mqttPort;
  String mqttServer = readString(&addr);
  cwConfig.mqttServer = mqttServer;
  String cwTopic = readString(&addr);
  cwConfig.cwTopic = cwTopic;
  String callsign = readString(&addr);
  cwConfig.callsign = callsign;
  String ssid = readString(&addr);
  cwConfig.ssid = ssid;
  String password = readString(&addr);
  cwConfig.password = password;

  Serial.print("rom ssid:");
  Serial.println(String(ssid));
  Serial.print("conf ssid:");
  Serial.println(cwConfig.ssid);
}

void updateSsid(String ssid) {
  cwConfig.ssid = ssid;
}

void updatePassword(String password) {
  cwConfig.password = password;
}

void updateCallsign(String callsign) {
  cwConfig.callsign = callsign;
}

void writeString(int* addr, String str) {
  //写入字符串长度
  int strLen = str.length();
  writeInt(addr, strLen);
  //写入字符
  for (int i = 0; i < strLen; i++) {
    EEPROM.write(*addr + i, str[i]);
  }
  *addr += strLen;
}

String readString(int* addr) {
  int strLen = readInt(addr);
  String str;
  for (int i = 0; i < strLen; i++) {
    char readChar = EEPROM.read(*addr + i);
    str += readChar;
  }
  *addr += strLen;
  return str;
}

void writeInt(int* addr, int val) {
  EEPROM.put(*addr, val);
  *addr += sizeof(val);
}

int readInt(int* addr) {
  int val;
  EEPROM.get(*addr, val);
  *addr += sizeof(val);
  return val;
}

#endif