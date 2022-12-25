#ifndef _CW_CONFIG_H
#define _CW_CONFIG_H

/**
 *自定义参数区域
 */
//WIFI名字
const char* ssid = "***ssid***";
//WIFI密码
const char* password = "***wifi password***";
//mqtt服务器
const char* mqttServer = "broker-cn.emqx.io";
//mqtt服务器端口
const int mqttPort = 1883;
//呼号
const String callsign = "BG5AUN";
//topic(聊天室)
const char* cwTopic = "/cw/bg5aun";

#endif