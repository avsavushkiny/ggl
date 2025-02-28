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
  ggl.bitmapGray(5, 10, cd, 40, 40);
  ggl.bitmapGray(0, 50, grayScale, 256, 5);
  ggl.bitmapGray(0, 80, gs, 8, 2);
  ggl.writeGrayString(50, 20, "Experience", 12, 1);
  ggl.drawText(160, 20, "ABBA", font);
  ggl.displayGray();
  delay(5000);
}
