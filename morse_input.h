#ifndef _CLASS_MORSE_INPUT_H
#define _CLASS_MORSE_INPUT_H

#include "base_input.h"
#include <list>
#include <string>

using namespace std;

class MorseInput : public BaseInput {
private:
  //上一次输入
  static MorseInput* preInput;
  //当前输入
  static MorseInput* currInput;
  //检查输入空格
  static bool checkInputSpace;
  //本地输入
  static list<BaseInput>* localInputs;
  //buffer
  static list<MorseInput> bufferInputs;
  //触发时间
  unsigned long triggerTime = 0;
  //释放时间
  unsigned long releaseTime = 0;

  static void addBufferInput(MorseInput input);
  static void clearAllBufferInput();
public:
  MorseInput();
  MorseInput(short cost, short span);

  //开始输入
  void begin(unsigned long triggerTime);
  //输入结束
  void end(unsigned long releaseTime);
  //输入结束
  void end(unsigned long releaseTime, MorseInput* preInput);
  //构建BaseInput
  BaseInput buildBaseInput();

  //triggerTime
  unsigned long getTriggerTime();
  //releaseTime
  unsigned long getReleaseTime();

  //开始输入
  static void beginInput(unsigned long triggerTime);
  //输入结束
  static MorseInput* endInput(unsigned long releaseTime);
  //检查输入
  static string checkInput(int diTime);

  //保存本地输入
  static void addLocalInput(BaseInput input);
  //清空本地输入
  static void clearAllLocalInput();
  //获取所有本地输入
  static list<BaseInput>* getAllLocalInput();

  //转换单个输入
  static BaseInput convertInput(string inputStr);
  //转换-.--这类字符串
  static list<BaseInput> convertCode(string code, int diTime);
  //转换发送的消息
  static list<BaseInput> convert(string message);
  //转换输入为消息
  static string convert(list<BaseInput>* inputs);
};

#endif