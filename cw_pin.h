#ifndef _CW_PIN_H
#define _CW_PIN_H

/**
 * 接线说明(仅数据线，电源和GND自己怎么方便怎么来，注意电压要求。):
 *  显示器：
 *    D1(gpio5) - SCL
 *    D2(gpio4) - SDA
 *  蜂鸣器：
 *    D3(gpio0) - I/O
 *  按钮：
 *    D6(gpio12) - I/O
 *  输入设备：
 *    D7(gpio13) - I/O
 */

#define BEE_BUILTIN 0
#define BTN_BUILTIN 12
#define KEY_BUILTIN 13

#endif