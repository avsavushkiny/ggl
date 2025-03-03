#include "ggl.h"
#include "bitmaps.h"


GGL ggl; Bitmaps bmp; FPS fps;

void setup() 
{
  ggl.gray.begin();
}

void loop()
{

  // monochrome
  // ggl.clearBuffer(); // -->
  // ggl.writeString(0, 10, "Experience", 12, 1);
  // fps.updateFPS(); fps.drawFps(0, 20);
  // ggl.display(); // <--


  // gray
  ggl.gray.clearBuffer();
  ggl.gray.bitmap(0, 120, bmp.cd, 40, 40);
  ggl.gray.bitmap(40, 120, bmp.cd, 40, 40);
  ggl.gray.bitmap(80, 120, bmp.cd, 40, 40);
  ggl.gray.bitmap(120, 120, bmp.cd, 40, 40);
  ggl.gray.bitmap(160, 120, bmp.cd, 40, 40);
  ggl.gray.bitmap(200, 120, bmp.cd, 40, 40);
  ggl.gray.bitmap(0, 80, bmp.cd, 40, 40);

  // ggl.bitmapGray(10, 10, bmp.cursor_b, 7, 10);

  // ggl.writeGrayString(45, 0, "QWERTYUIOP 0123456789", 12, 1, ggl.BLACK);
  // ggl.writeGrayString(45, 10, "ASDFGHJKL., ZXCVBNM<>", 12, 1, ggl.DARK_GRAY);
  // ggl.writeGrayString(45, 20, "qwertyuiop  asdfghjkl", 12, 1, ggl.LIGHT_GRAY);
  // ggl.writeGrayString(45, 30, "zxcvbnm,. <>", 12, 1, ggl.LIGHT_GRAY);

  // // ggl.drawGrayPixel(0, 50, ggl.BLACK);
  // // ggl.drawGrayPixel(1, 50, ggl.DARK_GRAY);
  // // ggl.drawGrayPixel(2, 50, ggl.LIGHT_GRAY);

  // ggl.drawGrayFrame(45, 15, 20, 10, ggl.BLACK);
  // ggl.drawGrayFillFrame(70, 15, 20, 10, ggl.BLACK, ggl.WHITE);

  // ggl.drawGrayFillCircle(20, 55, 10, ggl.BLACK, ggl.BLACK);
  // ggl.drawGrayFillCircle(30, 55, 10, ggl.BLACK, ggl.DARK_GRAY);
  // ggl.drawGrayFillCircle(40, 55, 10, ggl.BLACK, ggl.LIGHT_GRAY);
  // ggl.drawGrayCircle(50, 55, 10, ggl.BLACK);

  // ggl.drawGrayTriangle(15,80, 45,90, 128,80, ggl.BLACK, ggl.DARK_GRAY);
  // ggl.drawGrayHLine(0, 150, 256, ggl.BLACK, 1);
  // ggl.drawGrayVLine(250, 0, 160, ggl.DARK_GRAY, 1);
  // ggl.drawGrayBox(160, 55, 50, 15, ggl.DARK_GRAY);

  // ggl.drawGrayLine(10, 90, 110, 75, ggl.BLACK);

  // Данные для таблицы (значения разделены запятыми)
  // const char *tableData = "1,2,3,4,5,6,7,8,9";

  // Рисуем таблицу 3x3 с ячейками 30x20 пикселей, черными границами, белым текстом и серым фоном
  // ggl.drawGrayTable(0, 50, 3, 3, 20, 20, tableData, ggl.BLACK, ggl.BLACK, ggl.LIGHT_GRAY);

  // Обновляем счетчик FPS
  fps.updateFPS();
  fps.drawGrayFPS(0, 10, ggl.gray.BLACK);
  ggl.gray.writeString(0, 30, "*0(12+23)-4g./cf,d", 10, 1, ggl.gray.BLACK);
  ggl.gray.writeString(0, 40, "QWERT(SUIU)OP ASDFGHJKL ZXCVBNM", 10, 1, ggl.gray.BLACK);
  ggl.gray.writeString(0, 50, "qwertyuiop asdfghjkl zxcvbnm", 10, 1, ggl.gray.DARK_GRAY);
  ggl.gray.writeString(0, 60, "0123456789 0123456789", 10, 1, ggl.gray.LIGHT_GRAY);
  
  ggl.gray.display();
}
