#include <ggl.h>
#include <bitmaps.h>

GGL ggl;
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

    // FPS
    fps.updateFPS();
    fps.drawGrayFPS(0, 0, ggl.gray.BLACK);

    // Primitives
    ggl.gray.drawCircle(20, 20, 15, ggl.gray.BLACK);
    ggl.gray.drawCircle(40, 20, 15, ggl.gray.DARK_GRAY);
    ggl.gray.drawCircle(60, 20, 15, ggl.gray.LIGHT_GRAY);

    ggl.gray.drawFillFrame(0, 40, 20, 10, ggl.gray.BLACK, ggl.gray.DARK_GRAY);
    ggl.gray.drawFillFrame(30, 40, 20, 10, ggl.gray.DARK_GRAY, ggl.gray.LIGHT_GRAY);
    ggl.gray.drawFrame(60, 40, 20, 10, ggl.gray.BLACK);

    ggl.gray.drawBox(0, 55, 30, 10, ggl.gray.BLACK);
    ggl.gray.drawBox(40, 55, 30, 10, ggl.gray.DARK_GRAY);
    ggl.gray.drawBox(80, 55, 30, 10, ggl.gray.LIGHT_GRAY);

    ggl.gray.drawHLine(0, 70, 128, ggl.gray.BLACK, 5);
    ggl.gray.drawHLine(0, 77, 128, ggl.gray.DARK_GRAY, 3);
    ggl.gray.drawHLine(0, 82, 128, ggl.gray.LIGHT_GRAY, 1);

    // Send Data
    ggl.gray.sendBuffer();
}