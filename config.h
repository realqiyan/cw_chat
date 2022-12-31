#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>

using namespace std;

class Config {
public:
  //mqtt服务器端口
  int mqttPort;
  //mqtt服务器
  string mqttServer;
  //topic(聊天室)
  string topic;
  //呼号
  string callsign;
  //WIFI名字
  string ssid;
  //WIFI密码
  string password;
  //级别 1-40
  int level;
  //配置版本（用于刷新EEPROM存储）
  long version;

  Config(int mqttPort, string mqttServer, string topic, string callsign, string ssid, string password, int level, long version);

  //初始化配置
  void init();
  //持久化配置
  bool save();
  //读取配置
  void read();
  // 更新wifi名字
  void updateSsid(string ssid);
  // 更新wifi密码
  void updatePassword(string password);
  // 更新呼号
  void updateCallsign(string callsign);
  // 更新级别
  void updateLevel(int level);
};

#endif