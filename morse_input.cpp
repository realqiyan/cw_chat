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

MorseInput::MorseInput(int cost, int span) {
  this->cost = cost;
  this->span = span;
  //反推时间
  this->triggerTime = millis();
  this->releaseTime = this->triggerTime + cost;
}

char MorseInput::getInput() {
  return MorseInput::calculateInput(this->cost);
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
  if (preInputParam != NULL) {
    int currSpan = this->triggerTime - preInputParam->releaseTime;
    if (currSpan > MorseInput::KEY_DAH_TIME * 3) {
      currSpan = MorseInput::KEY_DAH_TIME * 3;
    }
    this->span = currSpan;
  } else {
    //首次输入自动添加空格
    this->span = MorseInput::KEY_DAH_TIME;
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
  str = str + to_string(span) + "|" + to_string(cost) + ";";
  return str;
}

//开始输入
void MorseInput::beginInput(unsigned long triggerTime) {
  if (currInput == NULL) {
    currInput = new MorseInput();
    currInput->begin(millis());
    // Serial.print("beginInput currInput.triggerTime:");
    // Serial.println(currInput->triggerTime);
  }
}
//输入结束
MorseInput* MorseInput::endInput(unsigned long releaseTime) {
  // 结束
  if (currInput != NULL) {
    MorseInput* retVal;
    currInput->end(millis(), preInput);
    // Serial.print("endInput currInput.releaseTime:");
    // Serial.println(currInput->releaseTime);
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
        code = code + (*it).getInput();
      }
      clearAllBufferInput();
      checkInputSpace = true;
      return code;
    }
    char inputChar = MorseInput::calculateInput(cost);
    if (inputChar == '\n' && bufferInputs.size() == 0) {
      //输入结束
      if (preInput != NULL) {
        delete preInput;
        preInput = NULL;
      }
      checkInputSpace = true;
      return "\n";
    } else if (checkInputSpace && inputChar == ' ' && bufferInputs.size() == 0) {
      //识别空格
      checkInputSpace = false;
      return " ";
    }
  }
  return "";
}

char MorseInput::calculateInput(int cost) {
  char inputChar;
  // 超过一半就算- 小于一半就算.
  if (cost > MorseInput::KEY_DAH_TIME * 5) {
    inputChar = '\n';
  } else if (cost > MorseInput::KEY_DAH_TIME * 2) {
    inputChar = ' ';
  } else if (cost > MorseInput::KEY_DAH_TIME / 2) {
    inputChar = '-';
  } else if (cost > 0) {
    inputChar = '.';
  } else {
    inputChar = '\0';
  }
  return inputChar;
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
  string span = inputStr.substr(0, lastIdx);
  string cost = inputStr.substr(lastIdx + 1);
  MorseInput morseInput(stoi(cost), stoi(span));
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
    } else {
      continue;
    }
    int span = MorseInput::KEY_DAH_TIME / (i == 0 ? 1 : 3);
    MorseInput dida(cost, span);
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
