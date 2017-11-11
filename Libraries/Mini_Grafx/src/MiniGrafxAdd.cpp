// MiniGrafxAdd.cpp for Yahoo5NewsViewWithWeather

#include "MiniGrafxAdd.h"

MiniGrafxAdd::MiniGrafxAdd(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette) : MiniGrafx(driver, bitsPerPixel, palette) {
  this->driver = driver;
  this->width = driver->width();
  this->height = driver->height();
  this->palette = palette;
}

void MiniGrafxAdd::ILI9341_Copy(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t X, uint16_t Y){
  uint16_t xstart, xend, ystart, yend, xn, yn;
  int8_t xp, yp, xd, yd;

  xp = X - x0;		// calc move bits
  if(x0 > X) {		// if move left
    xstart = x0;	// copy start from left side
    xend = x1+1;	// copy end is right side
    xd = 1;			// x point increase
  } else {			// move right
    xstart = x1;	// copy start from right side
    if(xstart + xp >= width)	// if move dest out of range
      xstart = width - xp - 1;	// calc start point
    xend = x0-1;				// copy end is left side
    xd = -1;		// x point decrease
  }

  yp = Y - y0;		// calc move bits
  if(y0 > Y) {		// if move up
    ystart = y0;	// copy start from top side
    yend = y1+1;	// copy end is bottom side
    yd = 1;			// y point increase
  } else {			// move down
	ystart = y1;	// copy start from bottom side
	if(ystart +yp >= height)	// if move dest out of range
	  ystart = height - yp - 1;	// calc start point
	yend = y0-1;				// copy end is top side
	yd = -1;		// y point decrease
  }

  for(yn = ystart; yn != yend; yn += yd) {		// move y from start to end
    for(xn = xstart; xn != xend; xn += xd) {	// move x from start to end
      setColor(getPixel(xn, yn));				// get origin point color
      setPixel(xn + xp, yn + yp);				// put destination point color
    }
  }
}

void MiniGrafxAdd::ILI9341_8x16_Font_DisplayOut(uint8_t txtMax, uint16_t x0, uint16_t y0, uint16_t color, uint8_t Fnt[][16]) {
  uint8_t com[6];
  uint16_t xlv, ylv;  // x loop value, y loop value
  if( txtMax > 12) txtMax = 12;

  setColor(0);
  fillRect(x0, y0, txtMax * 8, 16);

  int i, j, k;
  uint8_t bt = 0b10000000;

  setColor(color);


  for(i=0; i<16; i++){
    ylv = y0 + i;
    for(j=0; j<txtMax; j++){
      bt = 0b10000000;
      for(k=0; k<8; k++){
        xlv = x0 + j*8 + k;
        if(k>0){
          bt = bt >> 1;
        }
        if((Fnt[j][i] & bt)>0){
          setPixel(x0+j*8+k,y0+i);
        }
      }
    }
  }

}

void MiniGrafxAdd::Time_Copy_V_Scroll(uint8_t Direction, uint8_t ZorH, uint8_t buf[2][16], uint8_t *SclCnt, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color){
  uint8_t i, k;
  uint8_t bbb = 0b10000000;
  uint8_t y;
  uint16_t xlv;	// x loop value

  switch( Direction ){
    case 0:
      ILI9341_Copy(x0, y0+1, x1, y1, x0, y0);

      y = y1;
      break;
    case 1:
      ILI9341_Copy(x0, y0, x1, y1-1, x0, y0+1);

      y = y0;
      break;
  }  


  switch( Direction ){
    case 0:
      for(k=0; k<ZorH; k++){
        bbb = 0b10000000;
        for(i=0; i<8; i++){
          xlv = x0 + i + k*8;
          if(i>0) bbb = bbb >> 1;
          if(( buf[k][*SclCnt] & bbb ) > 0){
            setColor(color);
          }else{
            setColor(0);
          }
            setPixel(x0 +i + k*8, y);
        }
      }
      break;
    case 1:
      for(k=0; k<ZorH; k++){
        bbb = 0b10000000;
        for(i=0; i<8; i++){
          xlv = x0 + i + k*8;
          if(i>0) bbb = bbb >> 1;
          if(( buf[k][15 - (*SclCnt)] & bbb ) > 0){
            setColor(color);
          }else{
            setColor(0);
          }
            setPixel(x0 +i + k*8, y);
        }
      }
      break;
  }

  (*SclCnt)++;
}
//*********** 時刻水平スクロール **********************
void MiniGrafxAdd::Time_Copy_H_Scroll(uint8_t Direction, uint8_t ZorH, uint8_t buf[2][16], uint8_t *SclCnt, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color){
  uint8_t i;
  uint8_t k = 0;
  uint8_t bbb = 0;
  uint16_t x;
  uint16_t ylv;	// y loop value

  switch( Direction ){
    case 0:
      ILI9341_Copy(x0+1, y0, x1, y1, x0, y0);

      x = x1;

      if((*SclCnt) < 8){
        bbb = 0b10000000 >> (*SclCnt);
      }else if((*SclCnt) >= 8){
        bbb = 0b10000000 >> ((*SclCnt)-8);
      }

      if((*SclCnt) < 8){
        k = 0;
      }else{
        k = 1;
      }

      break;
    case 1:
      ILI9341_Copy(x0, y0, x1-1, y1, x0+1, y0);

      x = x0;

      if((*SclCnt) < 8){
        bbb = 0b00000001 << (*SclCnt);
      }else if((*SclCnt) >= 8){
        bbb = 0b00000001 << ((*SclCnt) - 8);
      }

      if(ZorH == 2){
        if((*SclCnt) < 8){
          k = 1;
        }else{
          k = 0;
        }
      }else if(ZorH ==1){
        k = 0;
      }

      break;
  }

  for(i=0; i<16; i++){
	ylv = y0 + i;
    if((buf[k][i] & bbb) > 0){
      setColor(color);
    }else{
      setColor(0);
    }
    setPixel(x, y0 + i);
  }

  (*SclCnt)++;
}

//*********電光掲示板風スクロール 8x16ドット********************
bool MiniGrafxAdd::Scroller_8x16_RtoL4line(uint16_t y0, uint8_t num, uint8_t Zen_or_Han, uint8_t fnt_buf[2][16], uint8_t color){

  Copy_Scroll(y0, fnt_buf[_ZorH_cnt[num]], _scl_cnt[num], color);

  _scl_cnt[num]++;

  if(_scl_cnt[num] == 8){
    _scl_cnt[num] = 0;
    if(Zen_or_Han == 2){
      if(_ZorH_cnt[num] == 0){
        _ZorH_cnt[num] = 1;
        return false;
      }else{
        _ZorH_cnt[num] = 0;
        return true;
      }
    }else{
      _ZorH_cnt[num] = 0;
      return true;
    }
  }

  return false;
}

bool MiniGrafxAdd::Scroller_8x16_RtoL4line(uint16_t y0, uint8_t num, uint8_t Zen_or_Han, uint8_t *SclCnt, uint8_t *ZorHcnt, uint8_t fnt_buf[2][16], uint8_t color){

  Copy_Scroll(y0, fnt_buf[*ZorHcnt], *SclCnt, color);

  *SclCnt = *SclCnt + 1;

  if(*SclCnt == 8){
    *SclCnt = 0;
    if(Zen_or_Han == 2){
      if(*ZorHcnt == 0){
        *ZorHcnt = 1;
        return false;
      }else{
        *ZorHcnt = 0;
        return true;
      }
    }else{
      *ZorHcnt = 0;
      return true;
    }
  }

  return false;
}
//***************************
void MiniGrafxAdd::Copy_Scroll(uint16_t y0, uint8_t buf[16], uint8_t scl_cnt2, uint8_t color){
  uint16_t xld;	// x last dot
  uint16_t ylv;	// y loop value
  xld = driver->width() - 1;
  ILI9341_Copy(1, y0, xld, y0 + 15, 0, y0);

  uint8_t bbb = 0;
  if(scl_cnt2 < 8){
    bbb = 0b10000000 >> scl_cnt2;
  }

  for(uint8_t i=0; i<16; i++){
	ylv = y0 + i;
    if((buf[i] & bbb) > 0){
      setColor(color);
    }else{
      setColor(0);
    }
    setPixel(xld, ylv);
  }

}
