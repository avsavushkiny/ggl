#include "ggl.h"


GGL ggl;

void setup() 
{
  ggl.begin();
}

void loop()
{

  // monochrome
  /*ggl.clearBuffer();
  ggl.drawXBMP(106, 58, 44, 44, ex_m);
  ggl.writeString(0, 10, "Experience", 12, 1);
  ggl.display();
  delay(5000);*/

  // gray
  ggl.clearBuffer();
  ggl.bitmapGray(0, 0, cd, 40, 40);
  ggl.bitmapGray(0, 155, grayScale, 256, 5);

  ggl.writeGrayString(45, 0, "QWERTYUIOP 0123456789", 12, 1);
  ggl.writeGrayString(45, 10, "ASDFGHJKL., ZXCVBNM<>", 12, 1);
  ggl.writeGrayString(45, 20, "qwertyuiop  asdfghjkl", 12, 1);
  ggl.writeGrayString(45, 30, "zxcvbnm,. <>", 12, 1);

  ggl.drawCircle(10, 45, 10, 1);
  ggl.drawLine(25, 45, 45, 45, 1);

  ggl.displayGray();
  delay(5000);
}
