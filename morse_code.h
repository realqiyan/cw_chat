#ifndef _CLASS_MORSE_CODE_H
#define _CLASS_MORSE_CODE_H

#include <string>
using namespace std;

class MorseCode {
private:
  static MorseCode MORSE_CODE_ARRAY[];
public:
  MorseCode(char letter, string code);
  //字符
  char letter;
  //莫尔斯码
  string code;
  //根据莫尔斯码获取字符
  static char getLetter(string val);
  //根据字符获取莫尔斯码
  static string getMorseCode(char val);
};
#endif