#ifndef _CLASS_CONTROLLER_H
#define _CLASS_CONTROLLER_H

#include <PubSubClient.h>
#include "SSD1306Wire.h"
#include "display_conf.h"
#include "config.h"
#include "morse_input.h"
#include "morse_code.h"
#include "morse_koch.h"
#include <list>
#include <string>

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>  //esp32
#endif
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>  //esp8266
#endif

using namespace std;

class Controller {
private:

  //波形显示上边界
  const int WAVE_SHOW_LINE = 18;
  //波形数据
  int waveData[128];
  //是否命令模式
  bool cmdMode = false;
  //存储的命令
  String commond;

  //状态 -1未初始化 0开始联网 1已联网 2已连接服务器 -1联网失败 -2连接服务器失败
  int status = -1;
  //小标题：显示IP、时间等
  String subTitle = "©BG5AUN";

  SSD1306Wire* display;
  Config* config;
  PubSubClient* pubSubClient;
  int beePin;
  int keyPin;
  int btnPin;

  //显示配置
  DisplayConf lineConfigs[4] = {
    { 22, 20, Roboto_Slab_Bold_12, "" },
    { 32, 60, Roboto_Slab_Regular_10, "" },
    { 42, 20, Roboto_Slab_Bold_12, "" },
    { 52, 60, Roboto_Slab_Regular_10, "" }
  };

  //是否处于命令模式
  bool isCmdMode();
  //退出命令模式
  void exitCmdMode();
  //处理输入命令
  void parseCmd();
  //执行命令
  bool doCmd(String cmd, String param);

public:

  //显示器配置
  static const int SHOW_LETTER_LINE = 0;
  static const int SHOW_CODE_LINE = 1;
  static const int INPUT_LETTER_LINE = 2;
  static const int INPUT_CODE_LINE = 3;


  Controller(Config* config, SSD1306Wire* display, PubSubClient* pubSubClient, int beePin, int keyPin, int btnPin);
  ~Controller();

  //初始化
  bool init();
  //loop
  void loop();
  //刷新显示
  void refresh();
  //清除屏幕显示
  void clear();
  //追加显示内容
  void displayLine(int line, String txt);
  //更新显示内容
  void updateLine(int line, String txt);
  //输出波形
  void outputWave(const MorseInput* input);
  //输出信息
  void outputMessage(list<MorseInput> inputs);
  //更新网络状态
  void changeStatus(int status);
  //更新小标题
  void changeSubTitle(String subTitle);
  //喇叭发声
  void play(bool enable);
  //播放实际输入
  void play(const MorseInput* morseInput);
  //发送消息
  void sendMessage(list<MorseInput> inputs);
  //训练
  void startTraining();

  //消息回调
  void callback(char* topic, unsigned char* payload, unsigned int length);
};


#endif