#ifndef _DISPLAY_CONF_H
#define _DISPLAY_CONF_H

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

#define MAX_LINE 4

//显示配置
DisplayConf lineConfigArr[] = {
  { 18, 20, Roboto_Slab_Regular_12, "" },
  { 28, 60, Roboto_Slab_Regular_10, "" },
  { 41, 20, Roboto_Slab_Regular_12, "" },
  { 51, 60, Roboto_Slab_Regular_10, "" }
};

#endif