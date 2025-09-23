/* Copyright (c) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DEBUG_H
#define DEBUG_H
#include <mbed.h>

extern Serial dbCom;

#define MAIN_DEBUG 1                      //<----------print debug ham main
#define SIZEOF_DEBUG 0                     //<----------print> in kich o cua kieu du lieu
#define RTC_DEBUG 0                        //<----------print debug luc cau hinh RTC
#define MEMORY_DEBUG 0                     //<----------print debug module memory
#define SETTIME_DEBUG 0                    //<----------print debug module  DS3231 set new time   >
#define EXECUTE_FLASHING_SHOW_TIME_DEBUG 0 //<----------print debug thoi gian thuc hien chop vang>
#define EXECUTE_AUTO_SHOW_TIME_DEBUG 0     //<----------print debug che do auto>
#define OUTPUT_DEBUG 0                     //<-----------print debug xuat tin hieu ngo ra len man hinh>
#define DEBUG_SEEK_COUNTER 0               //<----------print bien diem chu ky tin hieu>
#define DEBUG_CONSOLE 0                    //<----------Xuat ra man hinh debug console>
#define DEBUG_SEARCH_SCHEDULER 0           //<---------->debug thong so cua giai thuat tim schedule
#define DEBUG_RAILWAY_SIGNAL 0             //<---------->cho phep debug tin hieu duong sat
#define DEBUG_PRINTF_CARD_SEARCH 1         //cho phep xuat cac thong tin card cong suat tim duoc ra man hinh
#define DEBUG_SENSOR_READ_BACKET 0         //<---------->xuat man hinh gia tri cam bien doc vao
#define DEBUG_CYCLE 1                     //<---------->xuat man hinh gia tri cam bien doc vao
#define DEBUG_GLCD 1                      // su dung lai function glcd debug console ra may tinh
#define DEBUG_MONITOR_COUNTDOWN 0   
#define DEBUG_MONITOR_COUNTDOWN_CARD_INDEPENDENT 0                      

#define debug(condition, fmt, ...)                \
      do                                          \
      {                                           \
            if (condition)                        \
                  dbCom.printf(fmt, __VA_ARGS__); \
      } while (0)

#endif