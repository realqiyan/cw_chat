#ifndef _DISPLAY_CONF_H
#define _DISPLAY_CONF_H

#include "fonts.h"
#include <string>

using namespace std;

struct DisplayConf {
  //定义每一行的起始行
  short cursor;
  //定义每一行最大字数
  short maxSize;
  //定义每一行字体
  const uint8_t* font;
  //默认展示内容
  string text;
};

#endif