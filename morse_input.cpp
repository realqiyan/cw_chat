#include "morse_input.h"

//#include "HardwareSerial.h"

list<MorseInput> _LocalInput;
list<MorseInput> MorseInput::LOCAL_INPUTS = _LocalInput;

MorseInput::MorseInput(char input, int cost, int span) {
  this->input = input;
  this->cost = cost;
  this->span = span;
}

void MorseInput::addLocalInput(MorseInput input) {
  LOCAL_INPUTS.push_back(input);
}

void MorseInput::clearAllLocalInput() {
  LOCAL_INPUTS.clear();
}

list<MorseInput> MorseInput::getAllLocalInput() {
  return LOCAL_INPUTS;
}

//转换
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

//转换
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
//转换
string MorseInput::convert(list<MorseInput> inputs) {
  string message;
  //使用迭代器输出list容器中的元素
  for (list<MorseInput>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
    message = message + (*it).input + "|" + to_string((*it).cost) + "|" + to_string((*it).span) + ";";
  }
  return message;
}