#include "ggl.h"
#include <WiFi.h>

GGL ggl;

void setup() 
{
  ggl.begin();
}

void loop() {

  //ggl.clear();
  //ggl.bitmapGray(0, 0, grayScale, 256, 5);

  /*ggl.clearBuffer();
  ggl.displayGray();
  ggl.drawGrayBMP(10, 10, 32, 32, icon_c);
  ggl.displayGray();
  delay(5000);*/
  
  ggl.clearBuffer();
  ggl.drawXBMP(106, 58, 44, 44, ex_m);

  ggl.writeString(0,10, "Experience", 12, 1);

  ggl.display();
  delay(5000);

  ggl.clearBuffer();
  ggl.bitmapGray(106,58, ex_g, 44, 44);

  ggl.writeGrayString(0,10, "Experience", 12, 1);

  ggl.displayGray();
  delay(5000);

  
}
