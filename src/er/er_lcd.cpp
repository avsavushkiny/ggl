#include "er_lcd.h"

int WIDTH;
int HEIGHT;
uint8_t DISPLAY_ROTATE;



void set_display_rotate(uint8_t ROTATE)
{
  DISPLAY_ROTATE = ROTATE;
  if ((ROTATE == ROTATE_0) || (ROTATE == ROTATE_180))
  {
    WIDTH = 256;
    HEIGHT = 160;
  }
  if ((ROTATE == ROTATE_90) || (ROTATE == ROTATE_270))
  {
    WIDTH = 256;
    HEIGHT = 160;
  }
}

void command(uint8_t cmd)
{
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_DC, LOW);
  SPI.transfer(cmd); // Hardward SPI
  // SPIWrite_byte(cmd);    //Software SPI
  digitalWrite(LCD_CS, HIGH);
}

void dat(uint8_t dat)
{
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_DC, HIGH);
  SPI.transfer(dat); // Hardward SPI
  // SPIWrite_byte(dat); //Software SPI
  digitalWrite(LCD_CS, HIGH);
}

void SPIWrite_byte(uint8_t dat)
{
  digitalWrite(LCD_SCK, LOW);
  for (int i = 0; i < 8; i++)
  {
    if ((dat & 0x80))
      digitalWrite(LCD_MOSI, HIGH);
    else
      digitalWrite(LCD_MOSI, LOW);

    dat <<= 1;
    digitalWrite(LCD_SCK, HIGH);

    digitalWrite(LCD_SCK, LOW);
  }
}

/*
void SPIWrite(int *buffer, int bufferLength) {
    int i;
    for (i = 0; i < bufferLength; i++) {
        SPI.transfer(buffer[i]);
    }
}
*/

void er_lcd_begin()
{
  uint16_t Contrast = 243;
  pinMode(LCD_BL, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_DC, OUTPUT);
  pinMode(LCD_CS, OUTPUT);

  pinMode(LCD_SCK, OUTPUT);
  pinMode(LCD_MOSI, OUTPUT);

  // SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RST, HIGH);
  delay(10);
  digitalWrite(LCD_RST, LOW);
  delay(10);
  digitalWrite(LCD_RST, HIGH);
  delay(10);

  command(0x30); // Extension Command 1
  command(0x94); // Sleep Out
  delay(50);

  command(0x31); // Extension Command 2
  command(0x32); // Analog Circuit Set
  dat(0x00);
  dat(0x01);
  dat(0x03);
  command(0x51); // Booster Level x10
  dat(0xFA);     // 8X

  command(0x30);
  command(0x75);
  dat(0x00);
  dat(0x28);
  command(0x15);
  dat(0x00);
  dat(0xFF); // xe 256
  command(0xBC);
  dat(0x00);
  dat(0xA6);

  command(0x30); // Extension Command 1
  command(0x20); // Power Control
  dat(0x0b);     // VB ON ; VR,VF ON

  command(0x81); // Vop Control
  dat(Contrast & 0x3F);
  dat((Contrast >> 6) & 0x07);

  command(0x0C); // Data Format Select     DO=1; LSB on top
  command(0xf0); // Display Mode
  dat(0x11);     // Monochrome Mode

  command(0xCA); // Display Control
  dat(0x00);
  dat(0x9f); // duty 160
  dat(0x00);
  command(0xBC); // ata Scan Direction
  dat(0x00);     // MY=0

  command(0xaf); // Display On*/

  /*command(0x30);
  command(0x94);
  command(0x31);
  command(0xD7);
  dat(0x9F);
  
  command(0x32);
  dat(0x00);
  dat(0x01);
  dat(0x00);

  command(0x20);
  dat(0x01);
  dat(0x03);
  dat(0x05);
  dat(0x07);
  dat(0x09);
  dat(0x0b);
  dat(0x0d);
  dat(0x10);
  dat(0x11);
  dat(0x13);
  dat(0x15);
  dat(0x17);
  dat(0x19);
  dat(0x1b);
  dat(0x1d);
  dat(0x1f);

  command(0x31); //ex1
  command(0xf0);
  dat(0x0f);
  dat(0x0f);
  dat(0x0f);
  dat(0x0f);

  command(0x30); //ex0
  command(0x75);
  dat(0x00);
  dat(0x28);
  command(0x15);
  dat(0x00);
  dat(0xFF);     //xe 256
  command(0xBC);
  dat(0x00);
  dat(0xA6);

  command(0xca); //control
  dat(0x00);
  dat(0x9f);     //duty 160
  dat(0x20);

  command(0xF0); //display mode
  dat(0x10);     //10-1bit
  
  command(0x81); //ev control
  dat(Contrast & 0x3F);
  dat((Contrast >> 6) & 0x07);

  command(0x20); //power control
  dat(0xd1);     //d0 regular, d1 follower, d3 bopste
  delay(20);
  command(0xaf); //display on*/
  

  digitalWrite(LCD_BL, LOW);
}

void er_lcd_clear(int *buffer)
{
  int i;
  for (i = 0; i < WIDTH * (HEIGHT/4); i++)
  {
    buffer[i] = 0;
  }
}

void er_lcd_pixel(int x, int y, char color, int *buffer)
{

  int point_temp;
  if (DISPLAY_ROTATE == ROTATE_0)
  {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
      return;
    }
  }
  else if (DISPLAY_ROTATE == ROTATE_90)
  {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
      return;
    }
    point_temp = x;
    x = 256 - y - 1;
    y = point_temp;
  }
  else if (DISPLAY_ROTATE == ROTATE_180)
  {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
      return;
    }
    x = 256 - x - 1;
    y = 160 - y - 1;
  }
  else if (DISPLAY_ROTATE == ROTATE_270)
  {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
      return;
    }
    point_temp = x;
    x = y;
    y = 160 - point_temp - 1;
  }

  // if(x > WIDTH || y > HEIGHT)return ;
  if (color)
    buffer[x + (y / 8) * 256] |= 1 << (y % 8);
  else
    buffer[x + (y / 8) * 256] &= ~(1 << (y % 8));
}

void er_lcd_char1616(uint8_t x, uint8_t y, uint8_t chChar, int *buffer)
{
  uint8_t i, j;
  uint8_t chTemp = 0, y0 = y, chMode = 0;

  for (i = 0; i < 32; i++)
  {
    chTemp = pgm_read_byte(&Font1612[chChar - 0x30][i]);
    for (j = 0; j < 8; j++)
    {
      chMode = chTemp & 0x80 ? 1 : 0;
      er_lcd_pixel(x, y, chMode, buffer);
      chTemp <<= 1;
      y++;
      if ((y - y0) == 16)
      {
        y = y0;
        x++;
        break;
      }
    }
  }
}

void er_lcd_char(unsigned char x, unsigned char y, char acsii, char size, char mode, int *buffer)
{
  unsigned char i, j, y0 = y;
  char temp;
  unsigned char ch = acsii - ' ';
  for (i = 0; i < size; i++)
  {
    if (size == 12)
    {
      if (mode)
        temp = pgm_read_byte(&Font1206[ch][i]);
      else
        temp = ~pgm_read_byte(&Font1206[ch][i]);
    }
    else
    {
      if (mode)
        temp = pgm_read_byte(&Font1608[ch][i]);
      else
        temp = ~pgm_read_byte(&Font1608[ch][i]);
    }
    for (j = 0; j < 8; j++)
    {
      if (temp & 0x80)
        er_lcd_pixel(x, y, 1, buffer);
      else
        er_lcd_pixel(x, y, 0, buffer);
      temp <<= 1;
      y++;
      if ((y - y0) == size)
      {
        y = y0;
        x++;
        break;
      }
    }
  }
}

void er_lcd_string(uint8_t x, uint8_t y, const char *pString, uint8_t Size, uint8_t Mode, int *buffer)
{
  while (*pString != '\0')
  {
    if (x > (WIDTH - Size / 2))
    {
      x = 0;
      y += Size;
      if (y > (HEIGHT - Size))
      {
        y = x = 0;
      }
    }

    er_lcd_char(x, y, *pString, Size, Mode, buffer);
    x += Size / 2;
    pString++;
  }
}

void er_lcd_char3216(uint8_t x, uint8_t y, uint8_t chChar, int *buffer)
{
  uint8_t i, j;
  uint8_t chTemp = 0, y0 = y, chMode = 0;

  for (i = 0; i < 64; i++)
  {
    chTemp = pgm_read_byte(&Font3216[chChar - 0x30][i]);
    for (j = 0; j < 8; j++)
    {
      chMode = chTemp & 0x80 ? 1 : 0;
      er_lcd_pixel(x, y, chMode, buffer);
      chTemp <<= 1;
      y++;
      if ((y - y0) == 32)
      {
        y = y0;
        x++;
        break;
      }
    }
  }
}

void er_lcd_bitmap(uint8_t x, uint8_t y, const uint8_t *pBmp, uint8_t chWidth, uint8_t chHeight, int *buffer)
{
  uint8_t i, j, byteWidth = (chWidth + 7) / 8;
  for (j = 0; j < chHeight; j++)
  {
    for (i = 0; i < chWidth; i++)
    {
      if (pgm_read_byte(pBmp + j * byteWidth + i / 8) & (128 >> (i & 7)))
      {
        er_lcd_pixel(x + i, y + j, 1, buffer);
      }
      else
        er_lcd_pixel(x + i, y + j, 0, buffer);
    }
  }
}

void er_lcd_display(int *pBuf)
{
  int page, i;

  command(0xf0); // Display Mode
  dat(0x10);     // Monochrome Mode

  command(0x15);
  dat(0x00);
  dat(0xff);
  command(0x75);
  dat(0x00);
  dat(0x28);
  command(0x5c);

  for (page = 0; page < 160 / 4; page++)
  {
    for (i = 0; i < 256; i++)
    {
      dat(pBuf[i + (page * 256)]);
    }
  }
}

void er_lcd_pixel_gray(int x, int y, char color, int *buffer)
{
  set_display_rotate(ROTATE_0);
  if (x > 256 || y > 160 * 2)
    return;
  if (color)
    buffer[x + (y / 8) * 256] |= 1 << (y % 8);
  else
    buffer[x + (y / 8) * 256] &= ~(1 << (y % 8));
}

/*
static GUI_CONST_STORAGE GUI_COLOR Colorsgray_pix[] = {
     0x00,0x55,0xAA,0xFF
};
*/

void er_lcd_bitmap_gray(uint8_t x, uint8_t y, const uint8_t *pBmp, uint8_t chWidth, uint8_t chHeight, int *buffer)
{
  uint16_t i, j, k;
  uint8_t page = chHeight * 2 / 8;

  for (k = 0; k < page; k++)
  {
    for (j = 0; j < chWidth; j++)
    {
      for (i = 0; i < 8; i++)
      {
        if (pgm_read_byte(pBmp + j + k * chWidth) & (0x01 << (i & 7)))
        {
          er_lcd_pixel_gray(x + j, y + i + k * 8, 1, buffer);
        }
      }
    }
  }
}

void er_lcd_display_gray(int *pBuf)
{
  uint8_t page, i;

  command(0xf0); // Display Mode
  dat(0x11);     // 2bit Mode

  command(0x15);
  dat(0x00);
  dat(0xff);
  command(0x75);
  dat(0x00);
  dat(0x28);
  command(0x5c);

  for (page = 0; page < 160 / 4; page++)
  {
    for (i = 0; i < 256; i++)
    {

      dat(pBuf[i + (page * 256)]);
    }
  }
}

void demo_game(int *buffer)
{
  uint16_t cs = 200;
  uint8_t r = 5;          // Radius
  uint8_t i = 3;          // Stepping variables
  uint8_t x = 5, y = 5;   // Starting position
  uint8_t dx = i, dy = i; // Stepping
  uint8_t x1, y1;         // Save the last position

  while (cs--)
  {
    drawCircle(x1, y1, r, 0, buffer);
    x1 = x, y1 = y; // Save location
    if ((x - r) < 0)
      dx = i;
    if ((x + r) > WIDTH - 1)
      dx = -i;
    if ((y - r) < 0)
      dy = i;
    if ((y + r) > HEIGHT - 1)
      dy = -i;
    x += dx;
    y += dy;
    delay(100);
    drawCircle(x1, y1, r, 1, buffer); // Disappear before a round
    er_lcd_pixel(x1, y1, 1, buffer);  // Locus
    er_lcd_display(buffer);
  }
}

void demo_sine(int *buffer)
{
  drawFastHLine(0, HEIGHT / 2, WIDTH, 1, buffer);
  drawFastVLine(WIDTH / 2, 0, HEIGHT, 1, buffer);

  drawSine(HEIGHT / 2, HEIGHT / 4, 5, 1, buffer);
  er_lcd_display(buffer);
  delay(1000);
  drawSine(HEIGHT / 2, HEIGHT / 3, 4, 1, buffer);
  er_lcd_display(buffer);
  delay(1000);
}

void drawSine(uint16_t y, uint16_t a, uint16_t n, uint16_t color, int *buffer)
{
  uint16_t x1 = 0, x2;
  uint16_t y1 = HEIGHT / 2, y2;
  for (x2 = 0; x2 < WIDTH; x2++)
  {
    y2 = y + (a * sin(0.0175 * n * x2));
    drawLine(x1, y1, x2, y2, color, buffer);
    x1 = x2;
    y1 = y2;
  }
}

// Draw a circle outline
void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color, int *buffer)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  er_lcd_pixel(x0, y0 + r, color, buffer);
  er_lcd_pixel(x0, y0 - r, color, buffer);
  er_lcd_pixel(x0 + r, y0, color, buffer);
  er_lcd_pixel(x0 - r, y0, color, buffer);

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    er_lcd_pixel(x0 + x, y0 + y, color, buffer);
    er_lcd_pixel(x0 - x, y0 + y, color, buffer);
    er_lcd_pixel(x0 + x, y0 - y, color, buffer);
    er_lcd_pixel(x0 - x, y0 - y, color, buffer);
    er_lcd_pixel(x0 + y, y0 + x, color, buffer);
    er_lcd_pixel(x0 - y, y0 + x, color, buffer);
    er_lcd_pixel(x0 + y, y0 - x, color, buffer);
    er_lcd_pixel(x0 - y, y0 - x, color, buffer);
  }
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color, int *buffer)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4)
    {
      er_lcd_pixel(x0 + x, y0 + y, color, buffer);
      er_lcd_pixel(x0 + y, y0 + x, color, buffer);
    }
    if (cornername & 0x2)
    {
      er_lcd_pixel(x0 + x, y0 - y, color, buffer);
      er_lcd_pixel(x0 + y, y0 - x, color, buffer);
    }
    if (cornername & 0x8)
    {
      er_lcd_pixel(x0 - y, y0 + x, color, buffer);
      er_lcd_pixel(x0 - x, y0 + y, color, buffer);
    }
    if (cornername & 0x1)
    {
      er_lcd_pixel(x0 - y, y0 - x, color, buffer);
      er_lcd_pixel(x0 - x, y0 - y, color, buffer);
    }
  }
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color, int *buffer)
{
  drawFastVLine(x0, y0 - r, 2 * r + 1, color, buffer);
  fillCircleHelper(x0, y0, r, 3, 0, color, buffer);
}

// Used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color, int *buffer)
{

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1)
    {
      drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color, buffer);
      drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color, buffer);
    }
    if (cornername & 0x2)
    {
      drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color, buffer);
      drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color, buffer);
    }
  }
}

// Bresenham's algorithm - thx wikpedia
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, int *buffer)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    swapxy(x0, y0);
    swapxy(x1, y1);
  }

  if (x0 > x1)
  {
    swapxy(x0, x1);
    swapxy(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      er_lcd_pixel(y0, x0, color, buffer);
    }
    else
    {
      er_lcd_pixel(x0, y0, color, buffer);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, int *buffer)
{
  drawFastHLine(x, y, w, color, buffer);
  drawFastHLine(x, y + h - 1, w, color, buffer);
  drawFastVLine(x, y, h, color, buffer);
  drawFastVLine(x + w - 1, y, h, color, buffer);
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color, int *buffer)
{
  // Update in subclasses if desired!
  drawLine(x, y, x, y + h - 1, color, buffer);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color, int *buffer)
{
  // Update in subclasses if desired!
  drawLine(x, y, x + w - 1, y, color, buffer);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, int *buffer)
{
  // Update in subclasses if desired!
  for (int16_t i = x; i < x + w; i++)
  {
    drawFastVLine(i, y, h, color, buffer);
  }
}

// Draw a rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, int *buffer)
{
  // smarter version
  drawFastHLine(x + r, y, w - 2 * r, color, buffer);         // Top
  drawFastHLine(x + r, y + h - 1, w - 2 * r, color, buffer); // Bottom
  drawFastVLine(x, y + r, h - 2 * r, color, buffer);         // Left
  drawFastVLine(x + w - 1, y + r, h - 2 * r, color, buffer); // Right
  // draw four corners
  drawCircleHelper(x + r, y + r, r, 1, color, buffer);
  drawCircleHelper(x + w - r - 1, y + r, r, 2, color, buffer);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color, buffer);
  drawCircleHelper(x + r, y + h - r - 1, r, 8, color, buffer);
}

// Fill a rounded rectangle
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, int *buffer)
{
  // smarter version
  fillRect(x + r, y, w - 2 * r, h, color, buffer);

  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color, buffer);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color, buffer);
}

// Draw a triangle
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, int *buffer)
{
  drawLine(x0, y0, x1, y1, color, buffer);
  drawLine(x1, y1, x2, y2, color, buffer);
  drawLine(x2, y2, x0, y0, color, buffer);
}

// Fill a triangle
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, int *buffer)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    swapxy(y0, y1);
    swapxy(x0, x1);
  }
  if (y1 > y2)
  {
    swapxy(y2, y1);
    swapxy(x2, x1);
  }
  if (y0 > y1)
  {
    swapxy(y0, y1);
    swapxy(x0, x1);
  }

  if (y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    drawFastHLine(a, y0, b - a + 1, color, buffer);
    return;
  }

  int16_t
      dx01 = x1 - x0,
      dy01 = y1 - y0,
      dx02 = x2 - x0,
      dy02 = y2 - y0,
      dx12 = x2 - x1,
      dy12 = y2 - y1;
  int32_t
      sa = 0,
      sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++)
  {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swapxy(a, b);
    drawFastHLine(a, y, b - a + 1, color, buffer);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++)
  {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swapxy(a, b);
    drawFastHLine(a, y, b - a + 1, color, buffer);
  }
}

void testLines(int *buffer)
{
  int x1, y1, x2, y2, w = WIDTH, h = HEIGHT;
  uint8_t color = 1;
  x1 = y1 = 0;
  y2 = h - 1;

  for (x2 = 0; x2 < w; x2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }

  delay(1000);
  er_lcd_clear(buffer);

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;

  for (x2 = 0; x2 < w; x2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }

  delay(1000);
  er_lcd_clear(buffer);

  x1 = 0;
  y1 = h - 1;
  y2 = 0;

  for (x2 = 0; x2 < w; x2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }

  delay(1000);
  er_lcd_clear(buffer);

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;

  for (x2 = 0; x2 < w; x2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    drawLine(x1, y1, x2, y2, color, buffer);
    er_lcd_display(buffer);
    delay(30);
  }

  delay(1000);
}

void testFastLines(int *buffer)
{

  int x, y, w = WIDTH, h = HEIGHT;

  for (y = 0; y < h; y += 5)
  {
    drawFastHLine(0, y, w, 1, buffer);
    er_lcd_display(buffer);
    delay(30);
  }
  for (x = 0; x < w; x += 5)
  {
    drawFastVLine(x, 0, h, 1, buffer);
    er_lcd_display(buffer);
    delay(30);
  }

  delay(1000);
}

void testRects(int *buffer)
{

  int n, i, i2,
      cx = WIDTH / 2,
      cy = HEIGHT / 2;

  n = min(WIDTH, HEIGHT);

  for (i = 2; i < n; i += 6)
  {
    i2 = i / 2;
    drawRect(cx - i2, cy - i2, i, i, 1, buffer);
    er_lcd_display(buffer);
    delay(150);
  }
}

void testFilledRects(int *buffer)
{

  int n, i, i2,
      cx = WIDTH / 2 - 1,
      cy = HEIGHT / 2 - 1;

  n = min(WIDTH, HEIGHT);
  for (i = n; i > 0; i -= 6)
  {
    i2 = i / 2;

    fillRect(cx - i2, cy - i2, i, i, 1, buffer);
    er_lcd_display(buffer);
    delay(150);
    er_lcd_clear(buffer);

    // Outlines are not included in timing results
    //   drawRect(cx-i2, cy-i2, i, i, 1, buffer);  er_lcd_display(buffer);  delay(150);   er_lcd_clear(buffer);
  }
}

void testFilledCircles(uint8_t radius, int *buffer)
{
  int x, y, w = WIDTH, h = HEIGHT, r2 = radius * 2;

  for (x = radius; x < w; x += r2)
  {
    for (y = radius; y < h; y += r2)
    {
      fillCircle(x, y, radius, 1, buffer);
      er_lcd_display(buffer);
      delay(50);
    }
  }
}

void testCircles(uint8_t radius, int *buffer)
{
  int x, y, w = WIDTH, h = HEIGHT, r2 = radius * 2;

  for (x = radius; x < w; x += r2)
  {
    for (y = radius; y < h; y += r2)
    {
      drawCircle(x, y, radius, 1, buffer);
      er_lcd_display(buffer);
      delay(50);
    }
  }
}

void testTriangles(int *buffer)
{
  int n, i, cx = WIDTH / 2 - 1,
            cy = HEIGHT / 2 - 1;

  n = min(cx, cy);
  for (i = 0; i < n; i += 5)
  {
    drawTriangle(
        cx, cy - i,     // peak
        cx - i, cy + i, // bottom left
        cx + i, cy + i, // bottom right
        1, buffer);
    er_lcd_display(buffer);
    delay(150);
  }
}

void testFilledTriangles(int *buffer)
{
  int i, cx = WIDTH / 2 - 1,
         cy = HEIGHT / 2 - 1;

  for (i = min(cx, cy); i > 10; i -= 5)
  {

    fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, 1, buffer);
    er_lcd_display(buffer);
    delay(150);
    er_lcd_clear(buffer);

    //   drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,1, buffer);  er_lcd_display(buffer);  delay(150);   er_lcd_clear(buffer);
  }
}

void testRoundRects(int *buffer)
{
  int w, i, i2,
      cx = WIDTH / 2 - 1,
      cy = HEIGHT / 2 - 1;

  w = min(WIDTH, HEIGHT);
  for (i = 0; i < w; i += 6)
  {
    i2 = i / 2;
    drawRoundRect(cx - i2, cy - i2, i, i, i / 8, 1, buffer);
    er_lcd_display(buffer);
    delay(150);
  }
}

void testFilledRoundRects(int *buffer)
{
  int i, i2,
      cx = WIDTH / 2 - 1,
      cy = HEIGHT / 2 - 1;

  for (i = min(WIDTH, HEIGHT); i > 20; i -= 6)
  {
    i2 = i / 2;
    fillRoundRect(cx - i2, cy - i2, i, i, i / 8, 1, buffer);
    er_lcd_display(buffer);
    delay(150);
    er_lcd_clear(buffer);
  }
}
