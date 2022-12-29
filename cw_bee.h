#ifndef _CW_BEE_H
#define _CW_BEE_H

#include "morse_input.h"
#include "cw_config.h"

void initBee();
void bee(bool enable);
void playMorseCode(String morsecode);
void playMorseInput(const MorseInput* morseInput);

void initBee() {
  pinMode(BEE_BUILTIN, OUTPUT);
}

void bee(bool enable) {
  if (enable) {
    tone(BEE_BUILTIN, 600);
  } else {
    noTone(BEE_BUILTIN);
  }
}

void playMorseCode(String morsecode) {
  for (int i = 0; i < morsecode.length(); i++) {
    if (morsecode[i] == '.') {
      bee(true);
      delay(KEY_DAH_TIME / 3);
      bee(false);
    } else if (morsecode[i] == '-') {
      bee(true);
      delay(KEY_DAH_TIME);
      bee(false);
    } else if (morsecode[i] == ' ') {
      delay(KEY_DAH_TIME);
    }
    delay(KEY_DAH_TIME * 0.2);
  }
}

void playMorseInput(const MorseInput* morseInput) {
  int span = (*morseInput).span;
  if (span > 1000) {
    span = 1000;
  }
  delay(span);
  if ((*morseInput).input != ' ') {
    bee(true);
  }
  int cost = (*morseInput).cost;
  if (cost > 500) {
    cost = 500;
  }
  delay(cost);
  bee(false);
}
#endif