/*
 * CW练习&聊天室 v0.3 by BG5AUN
 *
 * 初始化请自行修改配置文件cw_config.h(配置呼号、WiFi、消息服务器)
 *
 * 硬件：ESP8266/esp32-S3开发板+i2c接口128*64显示器（ssd1306）+无源蜂鸣器+按钮+3.5毫米音频接口
 *
 * 接线说明查看README.md
 *
 * arduino api:https://www.arduino.cc/reference/en/
 * nodemcu: https://docs.ai-thinker.com/esp8266/boards/nodemcu
 * M.1677 : International Morse code 
 *    https://www.itu.int/rec/R-REC-M.1677-1-200910-I/en
 *    https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-C.pdf
 *  符号的间隔和长度：
 *    1 划是三个点的长度。
 *    2 构成同一字符的符号间的间隔是一个点的长度。
 *    3 字元之间的间隔是三个点的长度。
 *    4 单词之间的间隔是七个点的长度。
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
  Serial.println();
  delay(100);
  Serial.println("setup...");
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
