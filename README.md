# 基于Arduino的cw练习工具

## 依赖库
* PubSubClient
* ESP8266 and ESP32 OLED driver for SSD1306 displays
* Adafruit MQTT Library

## 接线说明
```
 CW练习&聊天室 Pro v0.3 by BG5AUN

 初始化请自行修改配置文件cw_config.h(配置呼号、WiFi、消息服务器)

 硬件：esp32-S3开发板（NodeMCU）+i2c接口128*64显示器（ssd1306）+无源蜂鸣器+按钮+3.5毫米音频接口

 esp32-S3接线说明(仅数据线，电源和GND自己怎么方便怎么来，注意电压要求。):
   显示器：
     6 - SCL
     7 - SDA
   蜂鸣器：
     8 - I/O
   按钮：
     15 - I/O
   输入设备：
     12 - I/O

 NodeMCU v1.2接线说明(仅数据线，电源和GND自己怎么方便怎么来，注意电压要求。):
 nodemcu: https://docs.ai-thinker.com/esp8266/boards/nodemcu
  显示器：
    gpio14 - SCL
    gpio12 - SDA
  蜂鸣器：
    gpio4 - I/O
  按钮：
    gpio2 - I/O
  输入设备：
    gpio5 - I/O

```

esp32-S3接线图（黑色按钮接手键）
![image](https://user-images.githubusercontent.com/4379546/212467316-5092111c-2fd7-4d7b-8fc6-021e4d7fe6ee.png)


## 配置说明

可以使用莫尔斯码输入命令配置，联网后可以加入群聊。

初始化配置：
```
/S:***ssid***;     设置wifi名字(通过莫斯码输入，仅支持小写)
/P:***password***; 设置wifi密码(通过莫斯码输入，仅支持小写)
/C:XXX;            设置呼号(通过莫斯码输入，大写)
/L:1;              设置训练级别 1-40
/D:50;             设置点的单位时间50ms
/T;                训练
/BTN:0;            切换按钮功能 0:触发训练按钮 1:发报按钮
/SAVE;             保存更新
/RESET;            重置更新
/EXIT;             退出命令模式
```
