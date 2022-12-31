#ifndef _MORSE_INPUT_H
#define _MORSE_INPUT_H

#include <list>

using namespace std;

class MorseInput {
private:
  //本地输入
  static list<MorseInput> LOCAL_INPUTS;
public:
  // 嗒的时间
  static const int KEY_DAH_TIME = 150;
  //字符
  char input;
  //耗时
  int cost;
  //上次输入间隔
  int span;
  //保存本地输入
  static void addLocalInput(MorseInput input);
  //清空本地输入
  static void clearAllLocalInput();
  //获取所有本地输入
  static list<MorseInput> getAllLocalInput();
};

#endif