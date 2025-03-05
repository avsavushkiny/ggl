#include "ggl.h"
#include "bitmaps.h"

GGL ggl;
Bitmaps bmp;
FPS fps;

void setup()
{
  // Settings display
  ggl.gray.begin();
}

void loop()
{
  // Clear Data
  ggl.gray.clearBuffer();
  // ggl.gray.bitmap(0, 120, bmp.cd, 40, 40);

  // FPS
  fps.updateFPS();
  fps.drawGrayFPS(0, 0, ggl.gray.BLACK);

  ggl.gray.writeLine(0, 30, "*0(12+23)-4g./cf,d", 10, 1, ggl.gray.BLACK);
  ggl.gray.writeLine(0, 40, "QWERT(SUIU)OP ASDFGHJKL ZXCVBNM", 10, 1, ggl.gray.BLACK);
  ggl.gray.writeLine(0, 50, "qwertyuiop asdfghjkl zxcvbnm", 10, 1, ggl.gray.DARK_GRAY);
  ggl.gray.writeLine(0, 60, "0123456789 0123456789", 10, 1, ggl.gray.LIGHT_GRAY);
  ggl.gray.writeLine(0, 10, 1455, 10, 1, ggl.gray.DARK_GRAY);

  // Send Data
  ggl.gray.sendBuffer();
}