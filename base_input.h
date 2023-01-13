#ifndef _CLASS_BASE_INPUT_H
#define _CLASS_BASE_INPUT_H

#include <string>

using namespace std;

class BaseInput {
public:
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
  char getInput(int diTime) {
    return BaseInput::calculateInput(cost, diTime);
  }
  // M.1677 : International Morse code
  //    https://www.itu.int/rec/R-REC-M.1677-1-200910-I/en
  //    https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-C.pdf
  //符号的间隔和长度
  //1 划是三个点的长度。
  //2 构成同一字符的符号间的间隔是一个点的长度。
  //3 字元之间的间隔是三个点的长度。
  //4 单词之间的间隔是七个点的长度。
  static char calculateInput(short cost, int diTime) {
    char inputChar;
    // 超过一半就算- 小于一半就算.
    if (cost > diTime * 10) {
      inputChar = '\n';
    } else if (cost > diTime * 7) {
      inputChar = ' ';
    } else if (cost > diTime * 1.5) {
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