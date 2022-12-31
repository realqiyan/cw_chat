#ifndef _MORSE_KOCH_H
#define _MORSE_KOCH_H

class MorseKoch {
private:
  static char KOCH_TRAINER[];
public:
  //获取随机字符
  static char getRandomCharByLevel(int level);
};
#endif