/*
 * CW练习&聊天室 v0.1 by BG5AUN
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
#include "fonts.h"
#include "morse_code.h"
#include "morse_input.h"
#include "morse_koch.h"
#include "display_conf.h"

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>  //esp32
#endif
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>  //esp8266
#endif

#include <PubSubClient.h>
#include <string>

#include "SSD1306Wire.h"

using namespace std;

// 防止抖动
#define KEY_SHAKE_TIME 10

//检查输入
void checkInput(int keyVal);
//检查输入的编码
void checkInputCode();
//喇叭发声
void bee(bool enable);
//播放实际输入
void playMorseInput(const MorseInput* morseInput);
//练习功能
void startMorseKoch();
//初始化显示器
void initDisplay();
//追加显示内容
void displayLine(int line, String txt);
//更新网络状态
void changeNetworkStatus(int status);
//清除屏幕显示
void clearDisplay();
//更新小标题
void updateSubTitle(String subTitle);
//是否处于命令模式
bool isCmdMode(String* commond);
//退出命令模式
void exitCmdMode();
//处理输入命令
void processCmd(String* commond);
//执行命令
void doCmd(String cmd, String param);
//初始化网络
void initNetwork();
//检查网络状态
void processNetwork();
//消息回调
void callback(char* topic, unsigned char* payload, unsigned int length);
//发送消息
void sendMessage(list<MorseInput> inputs);
//发送消息
void displayMessage(list<MorseInput> inputs);

//显示控制器
SSD1306Wire display(0x3c, SDA_BUILTIN, SCL_BUILTIN);

//网络状态 -1未初始化 0开始联网 1已联网 2已连接服务器 -1联网失败 -2连接服务器失败
int networkStatus = -1;
//小标题：显示IP、时间等
String subTitle = "©BG5AUN";
//是否命令模式
bool cmdMode = false;

void setup() {
  Serial.begin(115200);
  pinMode(BEE_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT_PULLUP);
  pinMode(KEY_BUILTIN, INPUT_PULLUP);
  randomSeed(analogRead(0));
  cwConfig.init();
  initDisplay();
  initNetwork();
}


void loop() {
  int btnVal = digitalRead(BTN_BUILTIN);
  if (btnVal == LOW) {
    startMorseKoch();
  }
  int keyVal = digitalRead(KEY_BUILTIN);
  checkInput(keyVal);
  checkInputCode();
  processNetwork();
}

// 防止输入信号抖动
int preKeyVal = -1;
unsigned long preChangeTime = 0;
// 命令(不识别空格)
String commond = "";

void checkInput(int keyVal) {
  //屏蔽抖动
  if (preKeyVal == keyVal || millis() - preChangeTime < KEY_SHAKE_TIME) {
    return;
  }
  preChangeTime = millis();
  preKeyVal = keyVal;
  if (keyVal == 0) {
    bee(true);
    MorseInput::beginInput(millis());
  } else if (keyVal == 1) {
    bee(false);
    MorseInput* retVal = MorseInput::endInput(millis());
    if (retVal != NULL) {
      MorseInput saveVal = *retVal;
      if (!isCmdMode(&commond)) {
        //莫斯码输入
        MorseInput::addLocalInput(saveVal);
        displayLine(INPUT_CODE_LINE, String(retVal->input));
      }
    }
  }
}

void checkInputCode() {
  string code = MorseInput::checkInput();
  if (code == "") {
    return;
  }
  if (code == " ") {
    //莫斯码输入
    if (!isCmdMode(&commond)) {
      MorseInput spaceInput(' ', MorseInput::KEY_DAH_TIME, MorseInput::KEY_DAH_TIME);
      MorseInput::addLocalInput(spaceInput);
    }
    displayLine(INPUT_LETTER_LINE, " ");
    displayLine(INPUT_CODE_LINE, " ");
  } else if (code == "\n") {
    //命令模式清除input
    if (isCmdMode(&commond)) {
      MorseInput::clearAllLocalInput();
    }
    //用于发送信息
    processCmd(&commond);
  } else {
    char letter = MorseCode::getLetter(code.c_str());
    commond += letter;
    if (!isCmdMode(&commond)) {
      MorseInput spaceInput(' ', MorseInput::KEY_DAH_TIME / 3, MorseInput::KEY_DAH_TIME / 3);
      MorseInput::addLocalInput(spaceInput);
      displayLine(INPUT_LETTER_LINE, String(letter));
      // 其他字符输入时已经显示 此时仅需追加空格
      displayLine(INPUT_CODE_LINE, " ");
    } else {
      lineConfigArr[INPUT_LETTER_LINE].text = commond.c_str();
      lineConfigArr[INPUT_CODE_LINE].text = "";
      refreshDisplay();
    }
  }
}

void bee(bool enable) {
  if (enable) {
#ifdef RGB_BUILTIN
    neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);  // Green
#endif
    tone(BEE_BUILTIN, 600);
  } else {
#ifdef RGB_BUILTIN
    digitalWrite(RGB_BUILTIN, LOW);  // Turn the RGB LED off
#endif
    noTone(BEE_BUILTIN);
  }
}

void playMorseInput(const MorseInput* morseInput) {
  int span = (*morseInput).span;
  delay(span);
  if ((*morseInput).input != ' ') {
    bee(true);
  }
  int cost = (*morseInput).cost;
  delay(cost);
  bee(false);
}

void startMorseKoch() {
  //随机展示一批字符
  int count = lineConfigArr[0].maxSize / 2;
  int index;
  char letterChar;
  int level = cwConfig.level;

  //小标题显示训练课程
  String levelTitle = "level:";
  levelTitle += cwConfig.level;
  updateSubTitle(levelTitle);

  for (int i = 0; i < count; i++) {
    // A random number between min and max-1. Data type: long.
    index = random(0, level + 1);
    letterChar = MorseKoch::getRandomCharByLevel(cwConfig.level);
    //获取莫斯码
    string code = MorseCode::getMorseCode(letterChar);
    code += " ";
    list<MorseInput> inputs = MorseInput::convertCode(code);
    displayMessage(inputs);
  }
}

void refreshDisplay() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  //callsign
  display.setFont(Roboto_Slab_Bold_13);
  display.drawString(0, 0, String(cwConfig.callsign.c_str()));

  //subTitle + 网络状态
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(Roboto_Slab_Regular_10);
  String statusStr;
  statusStr = subTitle + "/" + String(networkStatus);
  display.drawString(128, 3, statusStr);

  //模拟电波
  //上边界
  display.drawLine(0, waveShowLine - 2, 127, waveShowLine - 2);
  display.drawLine(0, waveShowLine - 1, 127, waveShowLine - 1);
  //下边界
  display.drawLine(0, waveShowLine + 2, 127, waveShowLine + 2);
  display.drawLine(0, waveShowLine + 3, 127, waveShowLine + 3);
  //电波
  for (int16_t j = 0; j < display.getWidth(); j++) {
    if (waveData[j] == 0) {
      display.drawLine(j, waveShowLine, j, waveShowLine);
      display.drawLine(j, waveShowLine + 1, j, waveShowLine + 1);
    } else {
      display.clearPixel(j, waveShowLine);
      display.clearPixel(j, waveShowLine + 1);
    }
  }

  // line
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  String lineText;
  for (int i = 0; i < MAX_LINE; i++) {
    display.setFont(lineConfigArr[i].font);
    lineText = String(lineConfigArr[i].text.c_str());
    if (lineText.length() > lineConfigArr[i].maxSize) {
      //lineText = lineText.substring(lineText.length() - lineConfigArr[i].maxSize);
    }
    display.drawString(128, lineConfigArr[i].cursor, lineText);
  }
  display.display();
}


void initDisplay() {
  delay(30);
  display.init();
  display.flipScreenVertically();
  refreshDisplay();
}

void displayLine(int line, String txt) {
  if (line > MAX_LINE) {
    return;
  }
  lineConfigArr[line].text = (String(lineConfigArr[line].text.c_str()) + txt).c_str();
  refreshDisplay();
}

void clearDisplay() {
  for (int i = 0; i < MAX_LINE; i++) {
    lineConfigArr[i].text = "";
  }
  refreshDisplay();
}

void changeNetworkStatus(int status) {
  if (status != networkStatus) {
    networkStatus = status;
    refreshDisplay();
  }
}

void updateSubTitle(String text) {
  subTitle = text;
}

bool isCmdMode(String* commond) {
  if (cmdMode) {
    return cmdMode;
  }
  int idx = commond->indexOf("/");
  bool isCmdInput = idx == 0;
  cmdMode = isCmdInput;
  return isCmdInput;
}

void exitCmdMode() {
  cmdMode = false;
  clearDisplay();
}

// /S:***ssid***;     设置wifi名字(通过莫斯码输入，仅支持小写)
// /P:***password***; 设置wifi密码(通过莫斯码输入，仅支持小写)
// /C:XXX;            设置呼号(通过莫斯码输入，大写)
// /L:1;              设置训练级别 1-40
// /SAVE;             保存更新
// /RESET;            重置更新
// /EXIT;             退出命令模式
void doCmd(String cmd, String param) {
  String uParam = param;
  param.toLowerCase();
  String lParam = param;

  if (String("S") == cmd) {
    cwConfig.updateSsid(lParam.c_str());
  } else if (String("P") == cmd) {
    cwConfig.updatePassword(lParam.c_str());
  } else if (String("C") == cmd) {
    cwConfig.updateCallsign(uParam.c_str());
  } else if (String("L") == cmd) {
    cwConfig.updateLevel(uParam.toInt());
  } else if (String("SAVE") == cmd) {
    cwConfig.save();
    initNetwork();
    exitCmdMode();
  } else if (String("RESET") == cmd) {
    cwConfig.read();
    exitCmdMode();
  } else if (String("EXIT") == cmd) {
    exitCmdMode();
  }
}

void processCmd(String* commond) {
  //命令模式一直等待命令结束，非命令模式直接发送消息
  if (isCmdMode(commond)) {
    //命令还未结束就继续等待输入
    int endIdx = commond->indexOf(";");
    if (endIdx != -1) {
      int paramIdx = commond->indexOf(":");
      int len = commond->length();
      String cmd;
      String param;
      if (paramIdx != -1) {
        cmd = commond->substring(1, paramIdx);
        param = commond->substring(paramIdx + 1, len - 1);
      } else {
        cmd = commond->substring(1, len - 1);
      }
      *commond = "";
      doCmd(cmd, param);
    }
  } else {
    *commond = "";
    list<MorseInput> inputs = MorseInput::getAllLocalInput();
    sendMessage(inputs);
    MorseInput::clearAllLocalInput();
  }
}

WiFiClient espClient;
PubSubClient client(espClient);

void initNetwork() {
  changeNetworkStatus(0);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(String(cwConfig.ssid.c_str()));
  WiFi.begin(cwConfig.ssid.c_str(), cwConfig.password.c_str());
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (++count > 6) {
      WiFi.disconnect();
      changeNetworkStatus(-1);
      return;
    }
  }
  changeNetworkStatus(1);
  client.setServer(cwConfig.mqttServer.c_str(), cwConfig.mqttPort);
  client.setBufferSize(5120);
  client.setCallback(callback);
}

void processNetwork() {
  //没有链接wifi直接退出
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  if (!client.connected()) {
    changeNetworkStatus(-2);
    while (!client.connected()) {
      String clientId = "CwChat-" + String(cwConfig.callsign.c_str());
      if (client.connect(clientId.c_str())) {
        Serial.println("MQTT connected");
        client.subscribe(cwConfig.topic.c_str());
      } else {
        delay(3000);
      }
    }
  } else {
    changeNetworkStatus(2);
  }
  client.loop();
}

void callback(char* topic, unsigned char* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
  int index = msg.indexOf(":");
  if (index < 0) {
    return;
  }
  // 消息处理
  String sender = msg.substring(0, index);
  // 小标题显示发送者
  updateSubTitle('#' + sender);
  if (String(cwConfig.callsign.c_str()) == sender) {
    return;
  }
  String message = msg.substring(index + 1);
  list<MorseInput> msgList = MorseInput::convert(message.c_str());
  displayMessage(msgList);
}



void displayWave(const MorseInput* input) {
  //一个嘀占2个像素
  int pixelTime = MorseInput::KEY_DAH_TIME / 6;
  //转0
  int countSpan = input->span / pixelTime;
  //转1
  int countCost = input->cost / pixelTime;
  //空格统一按照一个像素处理
  if (input->input == ' ') {
    countCost = 0;
    countSpan = 1;
  }
  int newData[countSpan + countCost];
  for (int i = 0; i < countSpan; i++) {
    newData[i] = 0;
  }
  for (int i = 0; i < countCost; i++) {
    newData[i + countSpan] = 1;
  }
  //上边界
  display.drawLine(0, waveShowLine - 2, 127, waveShowLine - 2);
  display.drawLine(0, waveShowLine - 1, 127, waveShowLine - 1);
  //下边界
  display.drawLine(0, waveShowLine + 2, 127, waveShowLine + 2);
  display.drawLine(0, waveShowLine + 3, 127, waveShowLine + 3);
  for (int i = 0; i < (countSpan + countCost); i++) {
    for (int16_t j = 0; j < display.getWidth(); j++) {
      //左移一格 画线
      if (j < display.getWidth() - 1) {
        waveData[j] = waveData[j + 1];
      } else {
        waveData[j] = newData[i];
      }
      if (waveData[j] == 0) {
        display.drawLine(j, waveShowLine, j, waveShowLine);
        display.drawLine(j, waveShowLine + 1, j, waveShowLine + 1);
      } else {
        display.clearPixel(j, waveShowLine);
        display.clearPixel(j, waveShowLine + 1);
      }
    }
    display.display();
  }
}

void displayMessage(list<MorseInput> msgList) {
  //处理字符
  String inputMorseCode;
  for (list<MorseInput>::iterator it = msgList.begin(); it != msgList.end(); ++it) {
    //displayLine(SHOW_CODE_LINE, String(it->input));
    playMorseInput(&*it);

    //显示波形displayWave
    displayWave(&*it);

    char inputChar = (*it).input;
    if (inputChar == ' ') {
      // 连续两个嘀嗒空格就输出一个空格（字母间隔会有一个空格，延时过长也会有一个空格）
      String displayLetter;
      if (inputMorseCode == "") {
        displayLetter = " ";
      } else {
        char letterChar = MorseCode::getLetter(inputMorseCode.c_str());
        displayLetter = String(letterChar);
        inputMorseCode = "";
      }
      displayLine(SHOW_LETTER_LINE, displayLetter);
    } else {
      inputMorseCode += inputChar;
    }
  }
}

void sendMessage(list<MorseInput> inputs) {
  if (inputs.size() > 0) {
    string message;
    message = cwConfig.callsign.c_str();
    message = message + ":" + MorseInput::convert(inputs);
    char sendMessage[message.length()];
    strcpy(sendMessage, message.data());
    bool publishRet = client.publish(cwConfig.topic.c_str(), sendMessage);
  }
}
