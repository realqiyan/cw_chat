#ifndef _CW_DISPLAY_H
#define _CW_DISPLAY_H

#define MAX_LINE 4

#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "cw_config.h"
#include "display_conf.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

DisplayConf lineConfigArr[] = {
  { 30, 16, u8g2_font_pxplusibmvga8_mf, "" },
  { 39, 40, u8g2_font_helvR08_tf, "" },
  { 55, 16, u8g2_font_pxplusibmvga8_mf, "" },
  { 64, 40, u8g2_font_helvR08_tf, "" }
};

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

  // clear the graphcis buffer
  display.clearDisplay();
  //网络状态
  displayNetworkStatus();

  // title
  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setFont(u8g2_font_profont17_tf);
  // start writing at this position
  u8g2_for_adafruit_gfx.setCursor(0, 15);
  u8g2_for_adafruit_gfx.print(String(cwConfig.callsign.c_str()));

  //subTitle
  u8g2_for_adafruit_gfx.setFont(u8g2_font_profont10_tf);
  u8g2_for_adafruit_gfx.setCursor(68, 13);
  u8g2_for_adafruit_gfx.print(subTitle);

  // line
  String lineText;
  for (int i = 0; i < MAX_LINE; i++) {
    //u8g2_for_adafruit_gfx.setFont(getFont(i));
    u8g2_for_adafruit_gfx.setFont(lineConfigArr[i].font);
    u8g2_for_adafruit_gfx.setCursor(0, lineConfigArr[i].cursor);  // start writing at this position
    lineText = String(lineConfigArr[i].text.c_str());
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
  lineConfigArr[line].text = (String(lineConfigArr[line].text.c_str()) + txt).c_str();

  refreshDisplay();
}

void displayCmd(String cmd) {
  lineConfigArr[INPUT_LETTER_LINE].text = cmd.c_str();
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
  u8g2_for_adafruit_gfx.setFont(u8g2_font_open_iconic_www_2x_t);
  if (networkStatus == -1) {
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X04A);
  } else if (networkStatus == 0) {
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X04E);
  } else if (networkStatus == 1) {
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X048);
  } else if (networkStatus == 2) {
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X053);
  } else if (networkStatus == -1) {
    u8g2_for_adafruit_gfx.drawGlyph(110, 18, 0X045);
  } else if (networkStatus == -2) {
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