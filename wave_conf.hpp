#ifndef _WAVE_CONF_H
#define _WAVE_CONF_H

using namespace std;

class WaveConf {
public:
  //定义每一行的起始行
  short cursor;
  //定义长度
  short length;
  //显示类型 '0'黑色 '1'白色
  char type;
  //每一行波形数据
  char* data;

  WaveConf(short cursor, short length, char type) {
    this->cursor = cursor;
    this->length = length;
    this->type = type;
    this->data = new char[length];
    for (int i = 0; i < length; i++) {
      this->data[i] = '0';
    }
  }
  ~WaveConf() {
    if (data != NULL) {
      delete[] this->data;
      this->data = NULL;
    }
  }
};

#endif