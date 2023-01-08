#include "controller.h"
//#define DEBUG_LOG true
#define KEY_SHAKE_TIME 10

//平均64个字符 一个字符平均按4个点划算
#define MAX_SEND_INPUT 64 * 4

// 防止输入信号抖动
int preKeyVal = -1;
unsigned long preChangeTime = 0;


Controller::Controller(Config* config, SSD1306Wire* display, PubSubClient* pubSubClient, short beePin, short keyPin, short btnPin) {
  this->config = config;
  this->beePin = beePin;
  this->keyPin = keyPin;
  this->btnPin = btnPin;
  this->display = display;
  this->pubSubClient = pubSubClient;
  //字符显示配置
  this->lineConfigs[0] = { 23, 20, Roboto_Slab_Bold_13, "" };
  this->lineConfigs[1] = { 47, 20, Roboto_Slab_Bold_13, "" };
  //波形显示配置
  int displayWidth = display->getWidth();
  this->waveConfigs[0] = new WaveConf(20, displayWidth, '0');
  this->waveConfigs[1] = new WaveConf(46, displayWidth, '1');
}
Controller::~Controller() {
  for (int i = 0; i < MAX_WAVE_LINE; i++) {
    delete this->waveConfigs[i];
    this->waveConfigs[i] = NULL;
  }
}

bool Controller::init() {
  //显示器
  display->init();
  display->flipScreenVertically();

  //网络服务
  changeStatus(0);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(String(config->ssid.c_str()));
  WiFi.begin(config->ssid.c_str(), config->password.c_str());
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (++count > 6) {
      WiFi.disconnect();
      changeStatus(-1);
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    changeStatus(1);
    pubSubClient->setServer(config->mqttServer.c_str(), config->mqttPort);
    pubSubClient->setBufferSize(5120);
  }

  //刷新显示
  refresh();
  return true;
}

void Controller::loop() {
  //网络
  if (WiFi.status() == WL_CONNECTED) {
    if (!pubSubClient->connected()) {
      changeStatus(-2);
      while (!pubSubClient->connected()) {
        String clientId = "CwChat-" + String(config->callsign.c_str());
        if (pubSubClient->connect(clientId.c_str())) {
          Serial.println("MQTT connected");
          pubSubClient->subscribe(config->topic.c_str());
        } else {
          delay(3000);
        }
      }
    } else {
      changeStatus(2);
    }
    pubSubClient->loop();
  }

  //练习
  int btnVal = digitalRead(btnPin);
  if (btnVal == LOW) {
    startTraining();
  }

  //key
  int keyVal = digitalRead(keyPin);
  //手键处理
  if (preKeyVal != keyVal && millis() - preChangeTime > KEY_SHAKE_TIME) {
    preChangeTime = millis();
    preKeyVal = keyVal;
    if (keyVal == 0) {
      play(true);
      MorseInput::beginInput(millis());
    } else if (keyVal == 1) {
      play(false);
      MorseInput* retVal = MorseInput::endInput(millis());
      if (retVal != NULL) {
        MorseInput saveVal = *retVal;
        if (!isCmdMode()) {
          //莫斯码输入
          BaseInput baseInput = saveVal.buildBaseInput();
          MorseInput::addLocalInput(baseInput);
#ifdef DEBUG_LOG
          Serial.print("allLocalInput size:");
          Serial.println(MorseInput::getAllLocalInput->size());
#endif
          outputWave(INPUT_CODE_LINE, &baseInput);
        }
      }
    }
  }
  //输入转字符
  string code = MorseInput::checkInput();
  if (code != "") {
    if (code == " ") {
      displayLine(INPUT_LETTER_LINE, " ");
    } else if (code == "\n") {
      //命令模式清除input
      if (isCmdMode()) {
        MorseInput::clearAllLocalInput();
      }
      //用于发送信息
      parseCmd();
    } else {
      char letter = MorseCode::getLetter(code.c_str());
      commond += letter;
      if (!isCmdMode()) {
        displayLine(Controller::INPUT_LETTER_LINE, String(letter));
        // 其他字符输入时已经显示 此时仅需追加空格
      } else {
        updateLine(Controller::INPUT_LETTER_LINE, commond);
        refresh();
      }
    }
  }
}

void Controller::refresh() {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  //callsign
  display->setFont(Roboto_Slab_Bold_13);
  display->drawString(0, 0, String(config->callsign.c_str()));

  //subTitle + 网络状态
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(Roboto_Slab_Regular_10);
  String statusStr;
  statusStr = subTitle + "/" + String(status);
  display->drawString(128, 3, statusStr);

  //模拟电波

  WaveConf* waveConfig;
  for (int i = 0; i < MAX_WAVE_LINE; i++) {
    waveConfig = waveConfigs[i];
    short currCursor = waveConfig->cursor;
    if (waveConfig->type == '0') {
      //上边界
      display->drawLine(0, currCursor - 2, 127, currCursor - 2);
      display->drawLine(0, currCursor - 1, 127, currCursor - 1);
      //下边界
      display->drawLine(0, currCursor + 2, 127, currCursor + 2);
      display->drawLine(0, currCursor + 3, 127, currCursor + 3);
    }
    //电波
    for (int16_t j = 0; j < display->getWidth(); j++) {
      if (waveConfig->data[j] == waveConfig->type) {
        display->drawLine(j, currCursor, j, currCursor);
        display->drawLine(j, currCursor + 1, j, currCursor + 1);
      } else {
        display->clearPixel(j, currCursor);
        display->clearPixel(j, currCursor + 1);
      }
    }
  }

  // line
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String lineText;
  for (int i = 0; i < MAX_LETTER_LINE; i++) {
    display->setFont(lineConfigs[i].font);
    lineText = String(lineConfigs[i].text.c_str());
    if (lineText.length() > lineConfigs[i].maxSize) {
      lineText = lineText.substring(lineText.length() - lineConfigs[i].maxSize);
    }
    display->drawString(128, lineConfigs[i].cursor, lineText);
  }
  display->display();
}


void Controller::displayLine(int line, String txt) {
  if (line > MAX_LETTER_LINE) {
    return;
  }
  lineConfigs[line].text = (String(lineConfigs[line].text.c_str()) + txt).c_str();
  refresh();
}

void Controller::updateLine(int line, String txt) {
  if (line > MAX_LETTER_LINE) {
    return;
  }
  lineConfigs[line].text = txt.c_str();
}

void Controller::clear() {
  for (int i = 0; i < MAX_LETTER_LINE; i++) {
    lineConfigs[i].text = "";
  }
  refresh();
}

//更新网络状态
void Controller::changeStatus(int status) {
  if (this->status != status) {
    this->status = status;
    refresh();
  }
}
//更新小标题
void Controller::changeSubTitle(String subTitle) {
  this->subTitle = subTitle;
}
//喇叭发声
void Controller::play(bool enable) {
  if (enable) {
#ifdef RGB_BUILTIN
    neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);  // Green
#endif
    tone(beePin, 600);
  } else {
#ifdef RGB_BUILTIN
    digitalWrite(RGB_BUILTIN, LOW);  // Turn the RGB LED off
#endif
    noTone(beePin);
  }
}
//播放实际输入
void Controller::play(const BaseInput* morseInput) {
  delay(morseInput->span);
  play(true);
  delay(morseInput->cost);
  play(false);
}

void Controller::outputWave(int line, const BaseInput* input) {
  //一个嘀占2个像素
  int pixelTime = MorseInput::KEY_DAH_TIME / 6;
  //转0 不足一个像素算一个像素
  int countSpan = input->span / pixelTime;
  countSpan = countSpan == 0 ? 1 : countSpan;
  //转1 不足一个像素算一个像素
  int countCost = input->cost / pixelTime;
  countCost = countCost == 0 ? 1 : countCost;
  char newData[countSpan + countCost];
  for (int i = 0; i < countSpan; i++) {
    newData[i] = '0';
  }
  for (int i = 0; i < countCost; i++) {
    newData[i + countSpan] = '1';
  }
  WaveConf* waveConfig = waveConfigs[line];
  short displayCursor = waveConfig->cursor;
  for (int i = 0; i < (countSpan + countCost); i++) {
    for (int16_t j = 0; j < display->getWidth(); j++) {
      //左移一格 画线
      if (j < display->getWidth() - 1) {
        waveConfig->data[j] = waveConfig->data[j + 1];
      } else {
        waveConfig->data[j] = newData[i];
      }
      if (waveConfig->data[j] == waveConfig->type) {
        display->drawLine(j, displayCursor, j, displayCursor);
        display->drawLine(j, displayCursor + 1, j, displayCursor + 1);
      } else {
        display->clearPixel(j, displayCursor);
        display->clearPixel(j, displayCursor + 1);
      }
    }
    if (i % 3 == 0) {
      display->display();
    }
  }
  display->display();
}

void Controller::outputMessage(list<BaseInput> msgList) {
#ifdef DEBUG_LOG
  Serial.print("displayMessage:");
  Serial.println(MorseInput::convert(msgList).c_str());
#endif
  //处理字符
  String inputMorseCode;
  for (list<BaseInput>::iterator it = msgList.begin(); it != msgList.end(); ++it) {
    //displayLine(SHOW_CODE_LINE, String(it->getInput()));
    play(&*it);
    //显示波形displayWave
    outputWave(SHOW_CODE_LINE, &*it);
    char inputChar = it->getInput();
    int inputSpan = it->span;
    if (inputSpan > MorseInput::KEY_DAH_TIME * 1.5) {
      if (inputSpan > MorseInput::KEY_DAH_TIME * 2) {
        displayLine(SHOW_LETTER_LINE, " ");
      }
      if (inputMorseCode != "") {
        char letterChar = MorseCode::getLetter(inputMorseCode.c_str());
        displayLine(SHOW_LETTER_LINE, String(letterChar));
        inputMorseCode = "";
      }
    }
    inputMorseCode += inputChar;
  }
  //处理最后的输入
  if (inputMorseCode != "") {
    char letterChar = MorseCode::getLetter(inputMorseCode.c_str());
    displayLine(SHOW_LETTER_LINE, String(letterChar));
    inputMorseCode = "";
  }
  displayLine(SHOW_LETTER_LINE, " ");
}

void Controller::sendMessage(list<BaseInput>* inputs) {
  if (inputs->size() > 0) {
    if (inputs->size() > MAX_SEND_INPUT) {
      Serial.println("Input Message Overload.");
      return;
    }
    string message;
    message = config->callsign.c_str();
    message = message + ":" + MorseInput::convert(inputs);
    char sendMessage[message.length()];
    strcpy(sendMessage, message.data());
    bool publishRet = pubSubClient->publish(config->topic.c_str(), sendMessage);
  }
}

//训练
void Controller::startTraining() {
  //随机展示一批字符
  int count = lineConfigs[0].maxSize / 2;
  int index;
  char letterChar;
  int level = config->level;

  //小标题显示训练课程
  String levelTitle = "level:";
  levelTitle += level;
  changeSubTitle(levelTitle);

  for (int i = 0; i < count; i++) {
    index = random(0, level + 1);
    letterChar = MorseKoch::getRandomCharByLevel(level);
    //获取莫斯码
    string code = MorseCode::getMorseCode(letterChar);
    list<BaseInput> inputs = MorseInput::convertCode(code);
    outputMessage(inputs);
    delay(50);
  }
}

void Controller::callback(char* topic, unsigned char* payload, unsigned int length) {
#ifdef DEBUG_LOG
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
#endif
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
#ifdef DEBUG_LOG
  Serial.println(msg);
#endif
  int index = msg.indexOf(":");
  if (index < 0) {
    return;
  }
  // 消息处理
  String sender = msg.substring(0, index);
  // 小标题显示发送者
  changeSubTitle('#' + sender);
  if (String(config->callsign.c_str()) == sender) {
    return;
  }
  String message = msg.substring(index + 1);
  list<BaseInput> msgList = MorseInput::convert(message.c_str());
  outputMessage(msgList);
}

//是否处于命令模式
bool Controller::isCmdMode() {
  if (cmdMode) {
    return cmdMode;
  }
  int idx = commond.indexOf("/");
  bool isCmdInput = (idx == 0);
  cmdMode = isCmdInput;
  return isCmdInput;
}
//退出命令模式
void Controller::exitCmdMode() {
  cmdMode = false;
  clear();
}
//处理输入命令
void Controller::parseCmd() {
  //命令模式一直等待命令结束，非命令模式直接发送消息
  if (isCmdMode()) {
    //命令还未结束就继续等待输入
    int endIdx = commond.indexOf(";");
    if (endIdx != -1) {
      int paramIdx = commond.indexOf(":");
      int len = commond.length();
      String cmd;
      String param;
      if (paramIdx != -1) {
        cmd = commond.substring(1, paramIdx);
        param = commond.substring(paramIdx + 1, len - 1);
      } else {
        cmd = commond.substring(1, len - 1);
      }
      commond = "";
      doCmd(cmd, param);
    }
  } else {
    commond = "";
    list<BaseInput>* inputs = MorseInput::getAllLocalInput();
    sendMessage(inputs);
    MorseInput::clearAllLocalInput();
  }
}
// /S:***ssid***;     设置wifi名字(通过莫斯码输入，仅支持小写)
// /P:***password***; 设置wifi密码(通过莫斯码输入，仅支持小写)
// /C:XXX;            设置呼号(通过莫斯码输入，大写)
// /L:1;              设置训练级别 1-40
// /SAVE;             保存更新
// /RESET;            重置更新
// /EXIT;             退出命令模式
bool Controller::doCmd(String cmd, String param) {
  String uParam = param;
  param.toLowerCase();
  String lParam = param;
  if (String("S") == cmd) {
    config->updateSsid(lParam.c_str());
  } else if (String("P") == cmd) {
    config->updatePassword(lParam.c_str());
  } else if (String("C") == cmd) {
    config->updateCallsign(uParam.c_str());
  } else if (String("L") == cmd) {
    config->updateLevel(uParam.toInt());
  } else if (String("SAVE") == cmd) {
    config->save();
    exitCmdMode();
  } else if (String("RESET") == cmd) {
    config->read();
    exitCmdMode();
  } else if (String("EXIT") == cmd) {
    exitCmdMode();
  }
  return true;
}