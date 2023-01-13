#include "config.h"
#include <EEPROM.h>
#include "morse_input.h"
#include "base_input.h"
#include "morse_koch.h"

int configSize = 256;

void writeString(int* addr, string str);
string readString(int* addr);
void writeInt(int* addr, int val);
int readInt(int* addr);


Config::Config(int mqttPort, string mqttServer, string topic, string callsign, string ssid, string password, int level, int diTime, long version) {
  this->mqttPort = mqttPort;
  this->mqttServer = mqttServer;
  this->topic = topic;
  this->callsign = callsign;
  this->ssid = ssid;
  this->password = password;
  this->level = level;
  this->diTime = diTime;
  this->version = version;
}



void Config::init() {
  EEPROM.begin(configSize);

  int versionSize = sizeof(this->version);
  int addr = configSize - versionSize;
  int romVersion = readInt(&addr);

  if (romVersion >= this->version) {
    this->read();
  } else {
    this->save();
  }
}

bool Config::save() {
  //int|int|char[]|int|char[]|int|char[]|int|char[]|int|char[]
  //读取顺序必须和存储顺序一致
  int addr = 0;
  //写入mqttPort
  writeInt(&addr, this->mqttPort);
  //写入服务器配置
  writeString(&addr, this->mqttServer);
  //写入topic
  writeString(&addr, this->topic);
  //写入callsign
  writeString(&addr, this->callsign);
  //写入ssid
  writeString(&addr, this->ssid);
  //写入password
  writeString(&addr, this->password);
  //写入level
  writeInt(&addr, this->level);
  //写入diTime
  writeInt(&addr, this->diTime);

  // 存储标识
  int versionSize = sizeof(this->version);
  if (addr >= (configSize - versionSize)) {
    return false;
  }
  addr = configSize - versionSize;
  writeInt(&addr, this->version);
  return EEPROM.commit();
}
void Config::read() {
  int addr = 0;
  //读取顺序必须和存储顺序一致
  this->mqttPort = readInt(&addr);
  this->mqttServer = readString(&addr);
  this->topic = readString(&addr);
  this->callsign = readString(&addr);
  this->ssid = readString(&addr);
  this->password = readString(&addr);
  this->level = readInt(&addr);
  this->diTime = readInt(&addr);
}

void Config::updateSsid(string ssid) {
  this->ssid = ssid;
}

void Config::updatePassword(string password) {
  this->password = password;
}

void Config::updateCallsign(string callsign) {
  this->callsign = callsign;
}

void Config::updateLevel(int level) {
  this->level = level;
}


void Config::updateDiTime(int diTime) {
  this->diTime = diTime;
}

void writeString(int* addr, string str) {
  //写入字符串长度
  int strLen = str.length();
  writeInt(addr, strLen);
  //写入字符
  for (int i = 0; i < strLen; i++) {
    EEPROM.write(*addr + i, str[i]);
  }
  *addr += strLen;
}

string readString(int* addr) {
  int strLen = readInt(addr);
  string str;
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
