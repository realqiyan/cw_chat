#ifndef _CW_BUTTON_H
#define _CW_BUTTON_H


#include "cw_bee.h"
#include "cw_koch.h"
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
    int level = cwConfig.level;

    //小标题显示训练课程
    String levelTitle = "level:";
    levelTitle += cwConfig.level;
    updateSubTitle(levelTitle);

    for (int i = 0; i < count; i++) {
      // A random number between min and max-1. Data type: long.
      index = random(0, level + 1);
      letterChar = KOCH_TRAINER[index];
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