#ifndef _CW_KEY_H
#define _CW_KEY_H

#include "morse_code.h"
#include "morse_input.h"
#include "cw_commond.h"

// 防止抖动
#define KEY_SHAKE_TIME 10


// 最近触发时间
unsigned long triggerTime = 0;
// 输入结束时间
unsigned long releaseTime = 0;
// 输入间隔
int span = 0;
// 按压标记
int startPress = 0;
// 是否可以检查空格
bool checkSpace = false;
// 是否可以输出
bool output = false;
// 输入的编码
String keyCode = "";
// 命令(不识别空格)
String commond = "";

void initKey();
void checkMorseCode();
void checkKeyPress();
void checkKeyRelease();

void initKey() {
  pinMode(KEY_BUILTIN, INPUT_PULLUP);
}

// 识别字符
void checkMorseCode() {
  // 如果输入字符检查标识开启，则检查字符
  if (startPress == 0) {
    int cost = millis() - releaseTime;
    if (cost > KEY_DAH_TIME * 1.5 && keyCode != "") {
      char letter = getLetter(keyCode);
      commond += letter;
      keyCode = "";
      checkSpace = true;
      if (!isCmdMode(&commond)) {
        MorseInput input = { ' ', cost, cost };
        LOCAL_INPUTS.push_back(input);
        displayLine(INPUT_LETTER_LINE, String(letter));
        displayLine(INPUT_CODE_LINE, " ");
      } else {
        displayCmd(commond);
      }
      //确认字符后更新释放时间
      releaseTime = millis();
    }
  }

  // 检查空格
  if (checkSpace) {
    int cost = millis() - releaseTime;
    if (cost > KEY_DAH_TIME * 3) {
      checkSpace = false;
      // 加上分割用的空格
      keyCode = "";
      output = true;

      //命令模式检查空格 但是不输出空格
      if (!isCmdMode(&commond)) {
        MorseInput input = { ' ', KEY_DAH_TIME * 3, cost };
        LOCAL_INPUTS.push_back(input);
        displayLine(INPUT_LETTER_LINE, " ");
        displayLine(INPUT_CODE_LINE, " ");
        //输入空格后更新释放时间
        releaseTime = millis();
      }
    }
  }

  // 整段结束检查
  if (output) {
    int cost = millis() - releaseTime;
    if (cost > KEY_DAH_TIME * 6) {
      output = false;
      //命令模式清除input
      if (isCmdMode(&commond)) {
        LOCAL_INPUTS.clear();
      }
      //用于发送信息
      processCmd(&commond);
    }
  }
}

// 检查按键是否按下
void checkKeyPress() {
  // 按键按下开始记录时间
  int cost = millis() - triggerTime;
  if ((cost > KEY_SHAKE_TIME || cost < 0)
      && digitalRead(KEY_BUILTIN) == 0
      && startPress == 0) {
    checkSpace = false;
    startPress = 1;
    triggerTime = millis();
    checkSpace = false;
    //上次放开按键后到目前的间隔
    span = millis() - releaseTime;
    // 处理异常情况
    if (span < 0) {
      span = 0;
    } else if (span > KEY_DAH_TIME * 3) {
      span = KEY_DAH_TIME * 3;
    }
    bee(true);
  }
}

// 检查按键是否释放
void checkKeyRelease() {
  int cost = millis() - triggerTime;
  if (digitalRead(KEY_BUILTIN) == 1 && startPress == 1) {
    if (cost > KEY_SHAKE_TIME) {
      startPress = 0;
      bee(false);
      char inputChar;
      // 超过一半就算- 小于一半就算.
      if (cost < KEY_DAH_TIME / 2) {
        inputChar = '.';
      } else if (cost >= KEY_DAH_TIME / 2) {
        inputChar = '-';
      }
      String inputStr = String(inputChar);
      keyCode += inputStr;

      if (!isCmdMode(&commond)) {
        MorseInput input = { inputChar, cost, span };
        LOCAL_INPUTS.push_back(input);
        displayLine(INPUT_CODE_LINE, inputStr);
      }

      //避免抖动信号 将触发时间更新
      triggerTime = millis();
    }
    releaseTime = millis();
  }
}

#endif