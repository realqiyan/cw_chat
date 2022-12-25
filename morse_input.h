#ifndef _MORSE_INPUT_H
#define _MORSE_INPUT_H

#include <list>

using namespace std;

struct MorseInput {
  //字符
  char input;
  //耗时
  int cost;
  //上次输入间隔
  int span;
};

list<MorseInput> inputs;


#endif