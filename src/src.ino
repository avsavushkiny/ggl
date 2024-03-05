#include "ggl.h"

void setup()
{
    initial_lcd();
}

void loop()
{
    clear_screen();
    disp_256x160(1,1, PIC1);

    delay(150);
}