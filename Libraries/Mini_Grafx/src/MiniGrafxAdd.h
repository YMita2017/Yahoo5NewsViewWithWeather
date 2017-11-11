// MiniGrafxAdd.h for Yahoo5NewsViewWithWeather

#include "ILI9341_SPI.h"
#include "MiniGrafx.h"
#include <FS.h>
#include <SPIFFS.h>


#ifndef _MINI_GRAFXH_ADD_
#define _MINI_GRAFXH_ADD_

//#define DEBUG_MINI_GRAFX(...) Serial.printf( __VA_ARGS__ )

#ifndef DEBUG_MINI_GRAFX_ADD
#define DEBUG_MINI_GRAFX_ADD(...)
#endif

class MiniGrafxAdd : public MiniGrafx {

 public:
  MiniGrafxAdd(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette);
  void ILI9341_Copy(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t X, uint16_t Y);
  void ILI9341_8x16_Font_DisplayOut(uint8_t txtMax, uint16_t x0, uint16_t y0, uint16_t color, uint8_t Fnt[][16]);
  void Time_Copy_V_Scroll(uint8_t Direction, uint8_t ZorH, uint8_t buf[2][16], uint8_t *SclCnt, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color);
  void Time_Copy_H_Scroll(uint8_t Direction, uint8_t ZorH, uint8_t buf[2][16], uint8_t *SclCnt, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color);
  bool Scroller_8x16_RtoL4line(uint16_t y0, uint8_t num, uint8_t Zen_or_Han, uint8_t fnt_buf[2][16], uint8_t color);
  bool Scroller_8x16_RtoL4line(uint16_t y0, uint8_t num, uint8_t Zen_or_Han, uint8_t *SclCnt, uint8_t *ZorHcnt, uint8_t fnt_buf[2][16], uint8_t color);
  void Copy_Scroll(uint16_t y0, uint8_t buf[16], uint8_t scl_cnt2, uint8_t color);
  
 private:
  DisplayDriver *driver;
  uint16_t width, height;
  uint16_t color;
  uint8_t rotation;
  int16_t transparentColor = -1;
  uint8_t bitsPerPixel = 4;
  uint8_t bitShift = 1;
  uint16_t bufferSize = 0;
  uint16_t* palette = 0;
  uint8_t bitMask;
  uint8_t pixelsPerByte;
  const char *fontData = ArialMT_Plain_16;
  TEXT_ALIGNMENT textAlignment;
  uint8_t _scl_cnt[4] = {};
  uint8_t _ZorH_cnt[4] = {};

};

#endif
