#include "ggl.h"
#include "bitmaps.h"


GGL ggl; Bitmaps bmp; FPS fps;

void setup() 
{
  ggl.gray.begin();
}

void loop()
{
  // gray
  ggl.gray.clearBuffer();
  ggl.gray.bitmap(0, 120, bmp.cd, 40, 40);

  // Обновляем счетчик FPS
  fps.updateFPS();
  fps.drawGrayFPS(0, 0, ggl.gray.BLACK);
  ggl.gray.writeLine(0, 30, "*0(12+23)-4g./cf,d", 10, 1, ggl.gray.BLACK);
  ggl.gray.writeLine(0, 40, "QWERT(SUIU)OP ASDFGHJKL ZXCVBNM", 10, 1, ggl.gray.BLACK);
  ggl.gray.writeLine(0, 50, "qwertyuiop asdfghjkl zxcvbnm", 10, 1, ggl.gray.DARK_GRAY);
  ggl.gray.writeLine(0, 60, "0123456789 0123456789", 10, 1, ggl.gray.LIGHT_GRAY);
  ggl.gray.writeLine(0, 10, 1455, 10, 1, ggl.gray.DARK_GRAY);
  
  ggl.gray.sendBuffer();
}
