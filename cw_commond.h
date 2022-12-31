#ifndef _CW_COMMOND_H
#define _CW_COMMOND_H

#include "cw_network.h"
#include "cw_display.h"
#include "cw_config.h"

bool cmdMode = false;

//是否处于命令模式
bool isCmdMode(String* commond);
//退出命令模式
void exitCmdMode();
//处理输入命令
void processCmd(String* commond);
//执行命令
void doCmd(String cmd, String param);

bool isCmdMode(String* commond) {
  if (cmdMode) {
    return cmdMode;
  }
  int idx = commond->indexOf("/");
  bool isCmdInput = idx == 0;
  cmdMode = isCmdInput;
  return isCmdInput;
}

void exitCmdMode() {
  cmdMode = false;
  clearDisplay();
}

// /S:***ssid***;     设置wifi名字(通过莫斯码输入，仅支持小写)
// /P:***password***; 设置wifi密码(通过莫斯码输入，仅支持小写)
// /C:XXX;            设置呼号(通过莫斯码输入，大写)
// /L:1;              设置训练级别 1-40
// /SAVE;             保存更新
// /RESET;            重置更新
// /EXIT;             退出命令模式
void doCmd(String cmd, String param) {
  Serial.println(cmd);
  Serial.println(param);

  String uParam = param;
  param.toLowerCase();
  String lParam = param;

  if (String("S") == cmd) {
    cwConfig.updateSsid(lParam.c_str());
  } else if (String("P") == cmd) {
    cwConfig.updatePassword(lParam.c_str());
  } else if (String("C") == cmd) {
    cwConfig.updateCallsign(uParam.c_str());
  } else if (String("L") == cmd) {
    cwConfig.updateLevel(uParam.toInt());
  } else if (String("SAVE") == cmd) {
    cwConfig.save();
    initNetwork();
    exitCmdMode();
  } else if (String("RESET") == cmd) {
    cwConfig.read();
    exitCmdMode();
  } else if (String("EXIT") == cmd) {
    exitCmdMode();
  }
}

void processCmd(String* commond) {
  //命令模式一直等待命令结束，非命令模式直接发送消息
  if (isCmdMode(commond)) {
    //命令还未结束就继续等待输入
    int endIdx = commond->indexOf(";");
    if (endIdx != -1) {
      int paramIdx = commond->indexOf(":");
      int len = commond->length();
      String cmd;
      String param;
      if (paramIdx != -1) {
        cmd = commond->substring(1, paramIdx);
        param = commond->substring(paramIdx + 1, len - 1);
      } else {
        cmd = commond->substring(1, len - 1);
      }
      *commond = "";
      doCmd(cmd, param);
    }
  } else {
    *commond = "";
    list<MorseInput> inputs = MorseInput::getAllLocalInput();
    sendMessage(inputs);
    MorseInput::clearAllLocalInput();
  }
}
#endif