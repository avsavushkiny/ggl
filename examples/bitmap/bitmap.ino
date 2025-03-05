#include <ggl.h>
#include <bitmaps.h>

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

    // Bitmap
    ggl.gray.bitmap(0, 10, bmp.icon_gray_3, 40, 40);

    // FPS
    fps.updateFPS();
    fps.drawGrayFPS(0, 0, ggl.gray.BLACK);

    // Send Data
    ggl.gray.sendBuffer();
}