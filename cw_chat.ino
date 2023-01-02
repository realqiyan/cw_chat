/*
 * CW练习&聊天室 v0.2 by BG5AUN
 *
 * 初始化请自行修改配置文件cw_config.h(配置呼号、WiFi、消息服务器)
 *
 * 硬件：ESP8266/esp32-S3开发板+i2c接口128*64显示器（ssd1306）+无源蜂鸣器+按钮+3.5毫米音频接口
 *
 * 接线说明查看README.md
 *
 * arduino api:https://www.arduino.cc/reference/en/
 */
#include "cw_config.h"
#include "controller.h"

using namespace std;

//显示控制器
SSD1306Wire display(0x3c, SDA_BUILTIN, SCL_BUILTIN);
//MQTT
WiFiClient espClient;
PubSubClient client(espClient);
//控制器
Controller ctrl(&cwConfig, &display, &client, BEE_BUILTIN, KEY_BUILTIN, BTN_BUILTIN);

//MQTT回调
void callback(char* topic, unsigned char* payload, unsigned int length) {
  ctrl.callback(topic, payload, length);
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  pinMode(BEE_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT_PULLUP);
  pinMode(KEY_BUILTIN, INPUT_PULLUP);
  //初始化
  cwConfig.init();
  //设置回调
  client.setCallback(callback);
  //初始化控制器
  ctrl.init();
}

void loop() {
  ctrl.loop();
}
