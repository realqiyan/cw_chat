/*
 * CW聊天室v0.1 by BG5AUN
 *
 * 初始化请自行修改配置文件cw_config.h(配置呼号、WiFi、消息服务器)
 *
 * 硬件：esp8266开发板（NodeMCU）+i2c接口128*64显示器（ssd1306）+无源蜂鸣器+按钮+3.5毫米音频接口
 *
 * 接线说明(仅数据线，电源和GND自己怎么方便怎么来，注意电压要求。):
 *  显示器：
 *    D1(gpio5) - SCL
 *    D2(gpio4) - SDA
 *  蜂鸣器：
 *    D3(gpio0) - I/O
 *  按钮：
 *    D6(gpio12) - I/O
 *  输入设备：
 *    D7(gpio13) - I/O
 *
 * arduino api:https://www.arduino.cc/reference/en/
 */
#include "cw_pin.h"
#include "cw_config.h"
#include "cw_display.h"
#include "cw_button.h"
#include "cw_bee.h"
#include "cw_key.h"
#include "cw_network.h"


void setup() {
  Serial.begin(115200);
  initConfig();
  randomSeed(analogRead(0));
  initDisplay();
  initNetwork();
  initButton();
  initBee();
  initKey();
}

void loop() {
  checkButton();
  checkKeyPress();
  checkKeyRelease();
  checkMorseCode();
  checkNetwork();
}
