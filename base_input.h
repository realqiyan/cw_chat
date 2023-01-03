#ifndef _CLASS_BASE_INPUT_H
#define _CLASS_BASE_INPUT_H

#include <string>

using namespace std;

class BaseInput {
public:
  // 嗒的时间
  static const short KEY_DAH_TIME = 160;
  //按压时常
  short cost = 0;
  //与上次输入间隔
  short span = 0;
  BaseInput() {
  }
  BaseInput(short cost, short span) {
    this->cost = cost;
    this->span = span;
  }
  //根据耗时计算输入
  char getInput() {
    return BaseInput::calculateInput(cost);
  }

  static char calculateInput(short cost) {
    char inputChar;
    // 超过一半就算- 小于一半就算.
    if (cost > KEY_DAH_TIME * 5) {
      inputChar = '\n';
    } else if (cost > KEY_DAH_TIME * 2) {
      inputChar = ' ';
    } else if (cost > KEY_DAH_TIME / 2) {
      inputChar = '-';
    } else if (cost > 0) {
      inputChar = '.';
    } else {
      inputChar = '\0';
    }
    return inputChar;
  }

  //toString
  string toString() {
    string str;
    str = to_string(span) + "|" + to_string(cost) + ";";
    return str;
  }
};

#endif