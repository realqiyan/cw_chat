#ifndef _MORSE_INPUT_H
#define _MORSE_INPUT_H

#include <list>
#include <string>

using namespace std;

class MorseInput {
private:
  //上一次输入
  static MorseInput* preInput;
  //当前输入
  static MorseInput* currInput;
  //检查输入空格
  static bool checkInputSpace;
  //本地输入
  static list<MorseInput> localInputs;
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
  MorseInput(char input, int cost, int span);

  // 嗒的时间
  static const int KEY_DAH_TIME = 160;
  //字符
  char input;
  //耗时
  int cost = 0;
  //上次输入间隔
  int span = 0;

  //开始输入
  void begin(unsigned long triggerTime);
  //输入结束
  void end(unsigned long releaseTime);
  //输入结束
  void end(unsigned long releaseTime, MorseInput* preInput);
  //toString
  string toString();
  //triggerTime
  unsigned long getTriggerTime();
  //releaseTime
  unsigned long getReleaseTime();

  //开始输入
  static void beginInput(unsigned long triggerTime);
  //输入结束
  static MorseInput* endInput(unsigned long releaseTime);
  //检查输入
  static string checkInput();

  //保存本地输入
  static void addLocalInput(MorseInput input);
  //清空本地输入
  static void clearAllLocalInput();
  //获取所有本地输入
  static list<MorseInput> getAllLocalInput();

  //转换单个输入
  static MorseInput convertInput(string inputStr);
  //转换-.--这类字符串
  static list<MorseInput> convertCode(string code);
  //转换发送的消息
  static list<MorseInput> convert(string message);
  //转换输入为消息
  static string convert(list<MorseInput> inputs);
};

#endif