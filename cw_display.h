#ifndef _CW_DISPLAY_H
#define _CW_DISPLAY_H

#define MAX_LINE 4

#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "cw_config.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

struct DisplayConf {
  //定义每一行的起始行
  short cursor;
  //定义每一行最大字数
  short maxSize;
  //定义每一行字体
  const uint8_t* font;
  //默认展示内容
  String text;
};

DisplayConf lineConfigArr[] = {
  { 30, 16, u8g2_font_pxplusibmvga8_mf, "" },
  { 39, 40, u8g2_font_helvR08_tf, "" },
  { 55, 16, u8g2_font_pxplusibmvga8_mf, "" },
  { 64, 40, u8g2_font_helvR08_tf, "" }
};


const int SHOW_LETTER_LINE = 0;
const int SHOW_CODE_LINE = 1;
const int INPUT_LETTER_LINE = 2;
const int INPUT_CODE_LINE = 3;

//网络状态 -1未初始化 0开始联网 1已联网 2已连接服务器 -1联网失败 -2连接服务器失败
int networkStatus = -1;

//小标题：显示IP、时间等
String subTitle = "©BG5AUN";

//初始化显示器
void initDisplay();

//追加显示内容
void displayLine(int line, String txt);

//更新网络状态
void changeNetworkStatus(int status);

//输出内容
void displayNetworkStatus();

//清除屏幕显示
void clearDisplay();

//更新小标题
void updateSubTitle(String subTitle);


void refreshDisplay() {

  display.clearDisplay();  // clear the graphcis buffer
  displayNetworkStatus();

  // title
  u8g2_for_adafruit_gfx.setFont(u8g2_font_profont17_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(0, 15);                 // start writing at this position
  u8g2_for_adafruit_gfx.print(cwConfig.callsign);

  //subTitle
  u8g2_for_adafruit_gfx.setFont(u8g2_font_profont10_tf);
  u8g2_for_adafruit_gfx.setCursor(72, 13);
  u8g2_for_adafruit_gfx.print(subTitle);

  // line
  String lineText;
  for (int i = 0; i < MAX_LINE; i++) {
    //u8g2_for_adafruit_gfx.setFont(getFont(i));
    u8g2_for_adafruit_gfx.setFont(lineConfigArr[i].font);
    u8g2_for_adafruit_gfx.setCursor(0, lineConfigArr[i].cursor);  // start writing at this position
    lineText = lineConfigArr[i].text;
    if (lineText.length() < lineConfigArr[i].maxSize) {
      while (lineText.length() < lineConfigArr[i].maxSize) {
        lineText = " " + lineText;
      }
    } else {
      lineText = lineText.substring(lineText.length() - lineConfigArr[i].maxSize);
    }
    u8g2_for_adafruit_gfx.print(lineText);
  }
  display.display();
}


void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {
      ;  // Don't proceed, loop forever
    }
  } else {
    u8g2_for_adafruit_gfx.begin(display);
    Serial.println("SSD1306 init success");
    refreshDisplay();
  }
}


void displayLine(int line, String txt) {
  if (line > MAX_LINE) {
    return;
  }
  lineConfigArr[line].text += txt;

  refreshDisplay();
}

void displayCmd(String cmd) {
  lineConfigArr[INPUT_LETTER_LINE].text = cmd;
  lineConfigArr[INPUT_CODE_LINE].text = "";
  refreshDisplay();
}

void clearDisplay() {
  for (int i = 0; i < MAX_LINE; i++) {
    lineConfigArr[i].text = "";
  }
  refreshDisplay();
}

void displayNetworkStatus() {
  //https://github.com/olikraus/u8g2/wiki/fntgrpiconic#open_iconic_www_2x
  //网络状态 -1未初始化 0开始联网 1已联网 2已连接服务器 -1联网失败 -2连接服务器失败
  if (networkStatus == -1) {
    u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X04A);
  } else if (networkStatus == 0) {
    u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X04E);
  } else if (networkStatus == 1) {
    u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X048);
  } else if (networkStatus == 2) {
    u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X053);
  } else if (networkStatus == -1) {
    u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X045);
  } else if (networkStatus == -2) {
    u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X045);
  }
}


void changeNetworkStatus(int status) {
  if (status != networkStatus) {
    networkStatus = status;
    refreshDisplay();
  }
}

void updateSubTitle(String text) {
  subTitle = text;  
}
#endif
