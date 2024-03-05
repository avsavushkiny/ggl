#include <Arduino.h>
#include <SPI.h>

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
    transfer_data_lcd(0X10);    // 10=Monochrome Mode,11=4Gray
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

const char W_PIC1 = 96;
const char H_PIC1 = 192;
char PIC1[]  =
{
  0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0xC0, 0x61, 0x00, 0x00, 0x38, 0xC0, 
  0x00, 0xE0, 0x7F, 0x80, 0x00, 0xFC, 0xDD, 0x81, 0x00, 0x5F, 0x6F, 0x87, 
  0x80, 0xFF, 0xDD, 0x8A, 0xC0, 0xEF, 0x6F, 0x9F, 0xE0, 0x73, 0xDB, 0x4A, 
  0xF0, 0xDD, 0xBF, 0x7D, 0xF0, 0x7E, 0x58, 0x29, 0x78, 0x37, 0xF0, 0x3A, 
  0xF8, 0x1F, 0x60, 0x35, 0xFC, 0x15, 0xE0, 0x55, 0xFC, 0xFD, 0x7F, 0x7F, 
  0x7C, 0xB7, 0xDB, 0x6A, 0xFC, 0x6F, 0xBF, 0x77, 0xBE, 0xDB, 0x7B, 0x6D, 
  0xFE, 0xF7, 0xFF, 0x7F, 0xDF, 0x1F, 0x00, 0x00, 0xEF, 0x17, 0xC0, 0x5B, 
  0xFF, 0x3F, 0x60, 0x7F, 0xFF, 0x6F, 0xF0, 0x2E, 0xF7, 0xDF, 0xAF, 0x3B, 
  0xFF, 0x7F, 0x7F, 0x17, 0xD7, 0xFF, 0xD5, 0x0E, 0x8F, 0xFF, 0xBF, 0x07, 
  0x0E, 0xFE, 0xDF, 0x01, 0x0E, 0xF8, 0x7F, 0x00, 0x1E, 0xFC, 0x0F, 0x00, 
  0xFC, 0x0F, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00,
};