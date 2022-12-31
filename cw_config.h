#ifndef _CW_CONFIG_H
#define _CW_CONFIG_H
#include "config.h"

//引脚配置
const int BEE_BUILTIN = 0;
const int BTN_BUILTIN = 12;
const int KEY_BUILTIN = 13;

//显示器配置
const int SHOW_LETTER_LINE = 0;
const int SHOW_CODE_LINE = 1;
const int INPUT_LETTER_LINE = 2;
const int INPUT_CODE_LINE = 3;

//初始化请自行修改配置(配置呼号、WiFi、消息服务器)
Config cwConfig = Config(1883, "broker-cn.emqx.io", "/cw/bg5aun", "BG5AUN", "***ssid***", "***password***", 1, 1);

#endif