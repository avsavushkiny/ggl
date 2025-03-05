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

    // Write
    ggl.gray.writeLine(0, 10, "Hello world!", 10, 1, ggl.gray.BLACK);
    ggl.gray.writeLine(0, 20, "Hello world! Hello world!", 10, 1, ggl.gray.DARK_GRAY);
    ggl.gray.writeLine(0, 20, "Hello world! Hello world! Hello world!", 10, 1, ggl.gray.LIGHT_GRAY);

    // Send Data
    ggl.gray.sendBuffer();
}