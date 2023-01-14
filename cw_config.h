#ifndef _CW_CONFIG_H
#define _CW_CONFIG_H
#include "config.h"

#if defined(ARDUINO_ARCH_ESP32)
//esp32引脚配置
const int SCL_BUILTIN = 6;
const int SDA_BUILTIN = 7;
const int KEY_BUILTIN = 12;
const int BEE_BUILTIN = 8;
const int BTN_BUILTIN = 15;
#endif


#if defined(ARDUINO_ARCH_ESP8266)
//esp8266引脚配置【old】
// const int SCL_BUILTIN = 5;
// const int SDA_BUILTIN = 4;
// const int KEY_BUILTIN = 13;
// const int BEE_BUILTIN = 0;
// const int BTN_BUILTIN = 12;

//NodeMUC引脚配置
const int SCL_BUILTIN = 14;
const int SDA_BUILTIN = 12;
const int KEY_BUILTIN = 5;
const int BEE_BUILTIN = 4;
const int BTN_BUILTIN = 2;

#endif


//初始化请自行修改配置(配置呼号、WiFi、消息服务器)
Config cwConfig = Config(1883, "broker-cn.emqx.io", "/cw/bg5aun", "TEST", "***ssid***", "***password***", 1, 50, 20230113);

#endif