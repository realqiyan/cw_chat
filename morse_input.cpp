#include "morse_input.h"

#include "HardwareSerial.h"

//本地输入
list<MorseInput> _LocalInput;
list<MorseInput> MorseInput::localInputs = _LocalInput;

//字符缓存
list<MorseInput> _BufferInputs;
list<MorseInput> MorseInput::bufferInputs = _BufferInputs;
//检查空格输入
bool MorseInput::checkInputSpace = true;
//上一次输入
MorseInput* MorseInput::preInput = NULL;
//当前输入
MorseInput* MorseInput::currInput = NULL;

MorseInput::MorseInput() {
}

MorseInput::MorseInput(char input, int cost, int span) {
  this->input = input;
  this->cost = cost;
  this->span = span;
  //反推时间
  this->triggerTime = millis();
  this->releaseTime = this->triggerTime + cost;
}

//开始输入
void MorseInput::begin(unsigned long triggerTime) {
  this->triggerTime = triggerTime;
}
//输入结束
void MorseInput::end(unsigned long releaseTime) {
  this->end(releaseTime, preInput);
}
//输入结束
void MorseInput::end(unsigned long releaseTime, MorseInput* preInputParam) {
  this->releaseTime = releaseTime;
  this->cost = this->releaseTime - this->triggerTime;
  char inputChar;
  // 超过一半就算- 小于一半就算.
  if (this->cost < MorseInput::KEY_DAH_TIME / 2) {
    inputChar = '.';
  } else {
    inputChar = '-';
  }
  this->input = inputChar;
  if (preInputParam != NULL) {
    int currSpan = this->triggerTime - preInputParam->releaseTime;
    if (currSpan > MorseInput::KEY_DAH_TIME * 3) {
      currSpan = MorseInput::KEY_DAH_TIME * 3;
    }
    this->span = currSpan;
  } else {
    this->span = 0;
  }
}

//triggerTime
unsigned long MorseInput::getTriggerTime() {
  return this->triggerTime;
}
//releaseTime
unsigned long MorseInput::getReleaseTime() {
  return this->releaseTime;
}

//toString
string MorseInput::toString() {
  string str;
  str.push_back(input);
  str = str + "|" + to_string(cost) + "|" + to_string(span) + ";";
  return str;
}

//开始输入
void MorseInput::beginInput(unsigned long triggerTime) {
  if (currInput == NULL) {
    currInput = new MorseInput();
    currInput->begin(millis());
  }
}
//输入结束
MorseInput* MorseInput::endInput(unsigned long releaseTime) {
  // 结束
  if (currInput != NULL) {
    MorseInput* retVal;
    currInput->end(millis(), preInput);
    retVal = currInput;
    if (preInput != NULL) {
      delete preInput;
      preInput = NULL;
    }
    preInput = currInput;
    currInput = NULL;

    MorseInput saveVal = *retVal;
    addBufferInput(saveVal);

    return retVal;
  }
  return NULL;
}
//检查输入
string MorseInput::checkInput() {
  //当前没有输入 并且已经存在一个历史输入
  if (currInput == NULL && preInput != NULL) {
    int cost = millis() - preInput->getReleaseTime();
    //识别字符
    string code;
    if (cost > MorseInput::KEY_DAH_TIME * 1.5 && bufferInputs.size() > 0) {
      for (list<MorseInput>::iterator it = bufferInputs.begin(); it != bufferInputs.end(); ++it) {
        code = code + (*it).input;
      }
      clearAllBufferInput();
      checkInputSpace = true;
      return code;
    }
    if (cost > MorseInput::KEY_DAH_TIME * 6 && bufferInputs.size() == 0) {
      //输入结束
      delete preInput;
      preInput = NULL;
      checkInputSpace = true;
      return "\n";
    } else if (checkInputSpace && cost > MorseInput::KEY_DAH_TIME * 3 && bufferInputs.size() == 0) {
      //识别空格
      checkInputSpace = false;
      return " ";
    }
  }
  return "";
}


void MorseInput::addLocalInput(MorseInput input) {
  localInputs.push_back(input);
}
void MorseInput::clearAllLocalInput() {
  localInputs.clear();
}
list<MorseInput> MorseInput::getAllLocalInput() {
  return localInputs;
}

void MorseInput::addBufferInput(MorseInput input) {
  bufferInputs.push_back(input);
}
void MorseInput::clearAllBufferInput() {
  bufferInputs.clear();
}


MorseInput MorseInput::convertInput(string inputStr) {
  //input|cost|span
  //-|237|459
  int lastIdx = inputStr.find_last_of("|");
  char input = inputStr[0];
  string cost = inputStr.substr(2, lastIdx - 2);
  string span = inputStr.substr(lastIdx + 1);
  MorseInput morseInput(input, stoi(cost), stoi(span));
  //Serial.println(morseInput.input);
  return morseInput;
}

list<MorseInput> MorseInput::convertCode(string code) {
  list<MorseInput> msgList;
  for (int i = 0; i < code.length(); i++) {
    int cost;
    if (code[i] == '.') {
      cost = MorseInput::KEY_DAH_TIME / 3;
    } else if (code[i] == '-') {
      cost = MorseInput::KEY_DAH_TIME;
    } else if (code[i] == ' ') {
      cost = MorseInput::KEY_DAH_TIME;
    }
    MorseInput dida(code[i], cost, MorseInput::KEY_DAH_TIME / 3);
    msgList.push_back(dida);
  }
  return msgList;
}

list<MorseInput> MorseInput::convert(string message) {
  list<MorseInput> msgList;
  while (message.find_first_of(";") != -1) {
    int idx = message.find_first_of(";");
    string inputStr = message.substr(0, idx);
    //解析输入对象
    MorseInput morseInput = convertInput(inputStr);
    msgList.push_back(morseInput);
    message = message.substr(idx + 1);
  }
  return msgList;
}

string MorseInput::convert(list<MorseInput> inputs) {
  string message;
  //使用迭代器输出list容器中的元素
  for (list<MorseInput>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
    message = message + (*it).toString();
  }
  return message;
}
