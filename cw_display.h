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
  { 15, 10, u8g2_font_helvR12_tf, callsign },
  { 35, 16, u8g2_font_pxplusibmvga8_mf, "" },
  { 55, 16, u8g2_font_pxplusibmvga8_mf, "" },
  { 64, 40, u8g2_font_helvR08_tf, "" }
};

//初始化显示器
void initDisplay();

//追加显示内容
void displayLine(int line, String txt);

void refreshDisplay() {

  display.clearDisplay();  // clear the graphcis buffer

  u8g2_for_adafruit_gfx.setFontMode(1);             // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setFontDirection(0);        // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);  // apply Adafruit GFX color

  // title
  u8g2_for_adafruit_gfx.setFont(lineConfigArr[0].font);         // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(0, lineConfigArr[0].cursor);  // start writing at this position
  u8g2_for_adafruit_gfx.print(lineConfigArr[0].text);

  // line
  String lineText;
  for (int i = 1; i < MAX_LINE; i++) {
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

#endif
