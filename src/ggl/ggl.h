#include <Arduino.h>
#include <SPI.h>
#pragma once

#define lcd_rs     17 
#define lcd_reset  16 
#define lcd_cs1    5
#define lcd_sid    23
#define lcd_sclk   18


void transfer_command_lcd(int data1)
{
    char i;
    digitalWrite(lcd_cs1, 0);
    digitalWrite(lcd_rs, 0);

    for (i = 0; i < 8; i++)
    {
        digitalWrite(lcd_sclk, 0);
        if (data1 & 0x80)
            digitalWrite(lcd_sid, 1);
        else
            digitalWrite(lcd_sid, 0);
        digitalWrite(lcd_sclk, 1);
        data1 <<= 1;
    }
    digitalWrite(lcd_cs1, 1);
}

void transfer_data_lcd(int data1)
{
    char i;
    digitalWrite(lcd_cs1, 0);
    digitalWrite(lcd_rs, 1);
    for (i = 0; i < 8; i++)
    {
        digitalWrite(lcd_sclk, 0);
        if (data1 & 0x80)
            digitalWrite(lcd_sid, 1);
        else
            digitalWrite(lcd_sid, 0);
        digitalWrite(lcd_sclk, 1);
        data1 <<= 1;
    }
    digitalWrite(lcd_cs1, 1);
}

/* Initializing the LCD module */
void initial_lcd()
{
    uint16_t Contrast = 202;

    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    SPI.begin();
    
    pinMode(lcd_rs, OUTPUT);
    pinMode(lcd_reset, OUTPUT);
    pinMode(lcd_cs1, OUTPUT);
    pinMode(lcd_sid, OUTPUT);
    pinMode(lcd_sclk, OUTPUT);
    

    digitalWrite(lcd_reset, 0);
    delay(100);
    digitalWrite(lcd_reset, 1);
    delay(100);
    transfer_command_lcd(0x30); // EXT=0
    transfer_command_lcd(0x94); // Sleep out
    transfer_command_lcd(0x31); // EXT=1
    transfer_command_lcd(0xD7); // Autoread disable
    transfer_data_lcd(0X9F);    //
    transfer_command_lcd(0x32); // Analog SET
    transfer_data_lcd(0x00);    // OSC Frequency adjustment
    transfer_data_lcd(0x01);    // Frequency on booster capacitors->6KHz
    transfer_data_lcd(0x00);    // Bias=1/14
    transfer_command_lcd(0x20); // Gray Level
    transfer_data_lcd(0x01);
    transfer_data_lcd(0x03);
    transfer_data_lcd(0x05);
    transfer_data_lcd(0x07);
    transfer_data_lcd(0x09);
    transfer_data_lcd(0x0b);
    transfer_data_lcd(0x0d);
    transfer_data_lcd(0x10);
    transfer_data_lcd(0x11);
    transfer_data_lcd(0x13);
    transfer_data_lcd(0x15);
    transfer_data_lcd(0x17);
    transfer_data_lcd(0x19);
    transfer_data_lcd(0x1b);
    transfer_data_lcd(0x1d);
    transfer_data_lcd(0x1f);

    transfer_command_lcd(0x31); // EXT=1
    transfer_command_lcd(0xf0); // delay 0.5s
    transfer_data_lcd(0x0f);
    transfer_data_lcd(0x0f);
    transfer_data_lcd(0x0f);
    transfer_data_lcd(0x0f);

    transfer_command_lcd(0x30); // EXT=0
    transfer_command_lcd(0x75); // Page Address setting
    transfer_data_lcd(0X00);    // XS=0
    transfer_data_lcd(0X28);    // XE=159 0x28
    transfer_command_lcd(0x15); // Clumn Address setting
    transfer_data_lcd(0X00);    // XS=0
    transfer_data_lcd(0Xff);    // XE=256
    transfer_command_lcd(0xBC); // Data scan direction
    transfer_data_lcd(0x00);    // MX.MY=Normal
    transfer_data_lcd(0xA6);
    transfer_command_lcd(0xCA); // Display Control
    transfer_data_lcd(0X00);    //
    transfer_data_lcd(0X9F);    // Duty=160
    transfer_data_lcd(0X20);    // Nline=off
    transfer_command_lcd(0xF0); // Display Mode
    transfer_data_lcd(0X11);    // 10=Monochrome Mode,11=4Gray
    transfer_command_lcd(0x81); // EV control
    transfer_data_lcd(0x2b);    // Fine-tuning-the-contrast value and setting the range 0x00～0x3f
    transfer_data_lcd(0x04);    // Coarse-contrast adjustment with adjustable range 0x00～0x07
    transfer_command_lcd(0x20); // Power control
    transfer_data_lcd(0x0B);    // D0=regulator ; D1=follower ; D3=booste, on:1 off:0
    delay(20);
    transfer_command_lcd(0xAF); // Display on
}

//transfer_command_lcd(0x95); //on  sleep LCD
//transfer_command_lcd(0x94); //off sleep LCD

/* Write */
void lcd_address(int x, int y, int x_total, int y_total)
{
    x = x - 1;
    y = y - 1;

    transfer_command_lcd(0x15); // Set Column Address
    transfer_data_lcd(x);
    transfer_data_lcd(x + x_total - 1);
    transfer_command_lcd(0x75); // Set Page Address
    transfer_data_lcd(y);
    transfer_data_lcd(y + y_total - 1);
    transfer_command_lcd(0x30);
    transfer_command_lcd(0x5c);
}

/* Clear buffer LCD */
void clear_screen()
{
    int i, j;
    lcd_address(0, 0, 256, 21);
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 256; j++)
        {
            transfer_data_lcd(0x00);
        }
    }
}

/* Bit-map */
void disp_256x160(int x, int y, char *dp)
{
    int i, j;
    lcd_address(x, y, 32, 32);
    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 32; j++)
        {
            transfer_data_lcd(*dp);
            dp++;
        }
    }
}

/* Bit-map matrix 32px32px */
void disp_32x215(int x, int y, char *dp)
{
    int i, j;
    lcd_address(x, y, 215, 4);
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 215; j++)
        {
            transfer_data_lcd(*dp);
            dp++;
        }
    }
}

/* Delay */
void delay(int i)
{
    int j, k;
    for (j = 0; j < i; j++)
        for (k = 0; k < 110; k++)
            ;
}
/* Delay */
void delay_us(int i)
{
    int j, k;
    for (j = 0; j < i; j++)
        for (k = 0; k < 1; k++)
        ;
}

const char W_PIC1 = 32;
const char H_PIC1 = 32;
char PIC1[]  =
{ 0X22,0X02,0X28,0X00,0X28,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X80,0X30,0X20,
    0X4C,0X48,0X53,0X53,0X53,0X5C,0X4C,0X70,0X30,0XC0,0XC0,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X80,0X30,0X20,
    0X4C,0X48,0X53,0X52,0X54,0X54,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
    0X55,0X55,0X54,0X57,0X53,0X5C,0X4C,0X70,0X30,0XC0,0XC0,0X00,0X00,0X00,0X00,0X00,
    0XA0,0X58,0X43,0X42,0X14,0X14,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
    0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
    0X55,0X55,0X54,0X57,0X53,0X5C,0XA8,0XE0,0XAA,0X55,0X55,0X55,0X55,0XE5,0XD4,0X94,
    0X91,0X51,0X45,0X45,0X15,0X15,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
    0X55,0X55,0X55,0X55,0X55,0X55,0X95,0X95,0XA5,0XA5,0XA9,0XA9,0XAA,0XAA,0XAA,0XFF,
    0X9A,0X25,0X95,0X95,0X55,0X50,0X51,0X4F,0X0F,0X2F,0X3E,0XBD,0XF9,0XF5,0XE4,0X94,
    0X55,0X51,0X45,0X45,0X15,0X55,0X95,0X95,0XA5,0XA5,0XA9,0XA9,0XAA,0XAA,0XAA,0XAA,
    0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XFF,0XAA,0X55,0X54,0X54,0XF2,0X32,0X89,0XC9,
    0X25,0X25,0X94,0X94,0X50,0X52,0X52,0X47,0X47,0X00,0X55,0X79,0X55,0X66,0XAA,0XAA,
    0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XEA,0XEA,0X7A,0X7A,0X9E,0X9E,0XA7,0XA7,0XFE,
    0X2A,0X95,0X55,0X55,0XDB,0X59,0X64,0X64,0X92,0X93,0X48,0X4C,0X22,0X32,0X89,0XC9,
    0X25,0X25,0X95,0X95,0X55,0X66,0XAA,0XEA,0XEA,0X7A,0X7A,0X9E,0X9E,0XA7,0XA7,0XA9,
    0XA9,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XFF,0X00,0X00,0X0E,0X3B,0X39,0XED,0XE7,0XB5,
    0X99,0XD5,0X56,0X56,0X59,0X59,0X64,0X64,0X92,0XFF,0X54,0X54,0X57,0X67,0XA7,0XA9,
    0XA9,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XEA,0XAA,0XBA,0XFA,0X0E,0X03,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X03,0X0E,0X0E,0X3B,0X39,0XED,0XE5,0XB5,
    0X95,0XDD,0X55,0X55,0X55,0X66,0XAA,0XAA,0XAA,0XAA,0XEA,0XAA,0XBA,0XEA,0XEE,0X3A,
    0X3B,0X0E,0X0E,0X03,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X03,0X0E,0X0E,0X3B,0X39,0XEE,0XEE,0X3A,
    0X3B,0X0E,0X0E,0X03,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    };
    