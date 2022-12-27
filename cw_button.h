#ifndef _CW_BUTTON_H
#define _CW_BUTTON_H


#include "cw_bee.h"
#include "morse_code.h"

void initButton();
void checkButton();


void initButton() {
  pinMode(BTN_BUILTIN, INPUT_PULLUP);
}

// 检查按键是否按下
void checkButton() {
  int btnVal = digitalRead(BTN_BUILTIN);
  if (btnVal == LOW) {
    //随机展示一批字符
    int count = lineConfigArr[0].maxSize / 2;
    int index;
    char letterChar;
    String morsecode;
    for (int i = 0; i < count; i++) {
      //0,25 26个字母  0,35 26个字母+10个数字
      index = random(0, 25);
      letterChar = morseCodeArr[index].letter;
      //获取莫斯码
      morsecode = getMorseCode(letterChar);
      //播放莫斯码
      playMorseCode(morsecode);
      //展示字母
      displayLine(SHOW_LETTER_LINE, String(letterChar));
      displayLine(SHOW_CODE_LINE, morsecode);
      //处理空格
      playMorseCode(" ");
      displayLine(SHOW_LETTER_LINE, " ");
      displayLine(SHOW_CODE_LINE, " ");
    }
  }
}
#endif