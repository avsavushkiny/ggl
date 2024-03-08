#include "ggl.h"

int _WIDTH;
int _HEIGHT;
int _DISPLAY_ROTATE;

int _LCD_BUFFER[256 * 160/4]; //10240

void GGL::rotate(int ROTATE)
{
  _DISPLAY_ROTATE = ROTATE;
  if ((ROTATE == ROTATE_0) || (ROTATE == ROTATE_180))
  {
    _WIDTH = 256;
    _HEIGHT = 160;
  }
  if ((ROTATE == ROTATE_90) || (ROTATE == ROTATE_270))
  {
    _WIDTH = 256;
    _HEIGHT = 160;
  }
}
void GGL::transferCommand(int cmd)
{
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_DC, LOW);
  SPI.transfer(cmd); // Hardward SPI
  // SPIWrite_byte(cmd);    //Software SPI
  digitalWrite(LCD_CS, HIGH);
}
void GGL::transferData(int dat)
{
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_DC, HIGH);
  SPI.transfer(dat); // Hardward SPI
  // SPIWrite_byte(dat); //Software SPI
  digitalWrite(LCD_CS, HIGH);
}
void GGL::SPIWrite_byte(int dat)
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

void GGL::begin()
{
  uint16_t Contrast = 240;
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

  transferCommand(0x30); // Extension Command 1
  transferCommand(0x94); // Sleep Out
  delay(50);

  transferCommand(0x31); // Extension Command 2
  transferCommand(0x32); // Analog Circuit Set
  transferData(0x00);
  transferData(0x01);
  transferData(0x03);
  transferCommand(0x51); // Booster Level x10
  transferData(0xFA);    // 8X

  transferCommand(0x30);
  transferCommand(0x75);
  transferData(0x00);
  transferData(0x28);
  transferCommand(0x15);
  transferData(0x00);
  transferData(0xFF);    // xe 256

  transferData(0xA6);

  transferCommand(0x30); // Extension Command 1
  transferCommand(0x20); // Power Control
  transferData(0x0b);    // VB ON ; VR,VF ON

  transferCommand(0x81); // Vop Control
  transferData(Contrast & 0x3F);
  transferData((Contrast >> 6) & 0x07);

  transferCommand(0x0C); // Data Format Select     DO=1; LSB on top
  transferCommand(0xf0); // Display Mode
  transferData(0x10);    // Monochrome Mode

  transferCommand(0xCA); // Display Control
  transferData(0x00);
  transferData(0x9f);    // duty 160
  transferData(0x00);

  transferCommand(0xBC); // Data Scan Direction
  transferData(0x00);    // MY=0

  transferCommand(0xaf); // Display On

  digitalWrite(LCD_BL, 0x0);
  //digitalWrite(LCD_BL, 1);
}
void GGL::display()
{
  int page, i;

  transferCommand(0xf0); // Display Mode
  transferData(0x10);     // Monochrome Mode

  transferCommand(0x15);
  transferData(0x00);
  transferData(0xff);
  transferCommand(0x75);
  transferData(0x00);
  transferData(0x28);
  transferCommand(0x5c);

  for (page = 0; page < 160 / 8; page++)
  {
    for (i = 0; i < 256; i++)
    {
      transferData(_LCD_BUFFER[i + (page * 256)]);
    }
  }
}
void GGL::clear()
{
  int i;
  for (i = 0; i <= _WIDTH * (_HEIGHT/4); i++)
  {
    _LCD_BUFFER[i] = 0;
  }
}
void GGL::pixel(int x, int y, char color)
{
  int point_temp;
  if (_DISPLAY_ROTATE == ROTATE_0)
  {
    if (x < 0 || x >= _WIDTH || y < 0 || y >= _HEIGHT)
    {
      return;
    }
  }
  else if (_DISPLAY_ROTATE == ROTATE_90)
  {
    if (x < 0 || x >= _WIDTH || y < 0 || y >= _HEIGHT)
    {
      return;
    }
    point_temp = x;
    x = 256 - y - 1;
    y = point_temp;
  }
  else if (_DISPLAY_ROTATE == ROTATE_180)
  {
    if (x < 0 || x >= _WIDTH || y < 0 || y >= _HEIGHT)
    {
      return;
    }
    x = 256 - x - 1;
    y = 160 - y - 1;
  }
  else if (_DISPLAY_ROTATE == ROTATE_270)
  {
    if (x < 0 || x >= _WIDTH || y < 0 || y >= _HEIGHT)
    {
      return;
    }
    point_temp = x;
    x = y;
    y = 160 - point_temp - 1;
  }

  // if(x > WIDTH || y > HEIGHT)return ;
  if (color)
    _LCD_BUFFER[x + (y / 8) * 256] |= 1 << (y % 8);
  else
    _LCD_BUFFER[x + (y / 8) * 256] &= ~(1 << (y % 8));
}
void GGL::bitmap(int x, int y, const int *pBmp, int chWidth, int chHeight)
{
  int i, j, byteWidth = (chWidth + 7) / 8;
  for (j = 0; j < chHeight; j++)
  {
    for (i = 0; i < chWidth; i++)
    {
      if (pgm_read_byte(pBmp + j * byteWidth + i / 8) & (128 >> (i & 7)))
      {
        GGL::pixel(x + i, y + j, 1);
      }
      else
        GGL::pixel(x + i, y + j, 0);
    }
  }
}
void GGL::pixelGray(int x, int y, char color)
{
  GGL::rotate(ROTATE_0);
  if (x > 256 || y > 256 * 2)
    return;
  if (color)
    _LCD_BUFFER[x + (y / 8) * 256] |= 1 << (y % 8);
  else
    _LCD_BUFFER[x + (y / 8) * 256] &= ~(1 << (y % 8));
}
void GGL::bitmapGray(int x, int y, const uint8_t *pBmp, int chWidth, int chHeight)
{
  int i, j, k;
  int16_t yy = y * 2;
  int page = chHeight * 2 / 8;

  for (k = 0; k < page; k++)
  {
    for (j = 0; j < chWidth; j++)
    {
      for (i = 0; i < 8; i++)
      {
        if (pgm_read_byte(pBmp + j + k * chWidth) & (0x01 << (i & 7)))
        {
          GGL::pixelGray(x + j, yy + i + k * 8, 1);
        }
      }
    }
  }
}
void GGL::displayGray()
{
  int page, i;

  transferCommand(0xf0); // Display Mode
  transferData(0x11);     // 4Gray  Mode

  transferCommand(0x15);
  transferData(0x00);
  transferData(0xff);
  transferCommand(0x75);
  transferData(0x00);
  transferData(0x28);
  transferCommand(0x5c);

  for (page = 0; page < 160/4; page++)
  {
    for (i = 0; i < 256; i++)
    {
      transferData(_LCD_BUFFER[i + (page * 256)]);
    }
  }
}

void GGL::writeChar1616(int x, int y, int chChar)
{
  int i, j;
  int chTemp = 0, y0 = y, chMode = 0;

  for (i = 0; i < 32; i++)
  {
    chTemp = pgm_read_byte(&Font1612[chChar - 0x30][i]);
    for (j = 0; j < 8; j++)
    {
      chMode = chTemp & 0x80 ? 1 : 0;
      GGL::pixel(x, y, chMode);
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
void GGL::writeChar(unsigned char x, unsigned char y, char acsii, char size, char mode)
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
        GGL::pixel(x, y, 1);
      else
        GGL::pixel(x, y, 0);
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
void GGL::writeString(int x, int y, const char *pString, int Size, int Mode)
{
  
  
  while (*pString != '\0')
  {
    if (x > (_WIDTH - Size / 2))
    {
      x = 0;
      y += Size;
      if (y > (_HEIGHT - Size))
      {
        y = x = 0;
      }
    }

    GGL::writeChar(x, y, *pString, Size, Mode);
    x += Size / 2;
    pString++;
  }
}
void GGL::writeChar3216(int x, int y, int chChar)
{
  int i, j;
  int chTemp = 0, y0 = y, chMode = 0;

  for (i = 0; i < 64; i++)
  {
    chTemp = pgm_read_byte(&Font3216[chChar - 0x30][i]);
    for (j = 0; j < 8; j++)
    {
      chMode = chTemp & 0x80 ? 1 : 0;
      GGL::pixel(x, y, chMode);
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
void GGL::writeGrayChar(unsigned char x, unsigned char y, char acsii, char size, char mode)
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
        GGL::pixel(x, y, 1);
      else
        GGL::pixel(x, y, 0);
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
void GGL::writeGrayString(int x, int y, const char *pString, int Size, int Mode)
{
  int yy = y * 2;
  
  while (*pString != '\0')
  {
    if (x > (_WIDTH - Size / 2))
    {
      x = 0;
      yy += Size;
      if (yy > (_HEIGHT - Size))
      {
        yy = x = 0;
      }
    }

    GGL::writeGrayChar(x, yy, *pString, Size, Mode);
    x += Size / 2;
    pString++;
  }
}


void GGL::drawSine(uint16_t y, uint16_t a, uint16_t n, uint16_t color)
{
  uint16_t x1 = 0, x2;
  uint16_t y1 = _HEIGHT / 2, y2;
  for (x2 = 0; x2 < _WIDTH; x2++)
  {
    y2 = y + (a * sin(0.0175 * n * x2));
    GGL::drawLine(x1, y1, x2, y2, color);
    x1 = x2;
    y1 = y2;
  }
}
void GGL::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  GGL::pixel(x0, y0 + r, color);
  GGL::pixel(x0, y0 - r, color);
  GGL::pixel(x0 + r, y0, color);
  GGL::pixel(x0 - r, y0, color);

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

    GGL::pixel(x0 + x, y0 + y, color);
    GGL::pixel(x0 - x, y0 + y, color);
    GGL::pixel(x0 + x, y0 - y, color);
    GGL::pixel(x0 - x, y0 - y, color);
    GGL::pixel(x0 + y, y0 + x, color);
    GGL::pixel(x0 - y, y0 + x, color);
    GGL::pixel(x0 + y, y0 - x, color);
    GGL::pixel(x0 - y, y0 - x, color);
  }
}
void GGL::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, int cornername, uint16_t color)
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
      GGL::pixel(x0 + x, y0 + y, color);
      GGL::pixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2)
    {
      GGL::pixel(x0 + x, y0 - y, color);
      GGL::pixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8)
    {
      GGL::pixel(x0 - y, y0 + x, color);
      GGL::pixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1)
    {
      GGL::pixel(x0 - y, y0 - x, color);
      GGL::pixel(x0 - x, y0 - y, color);
    }
  }
}
void GGL::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
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
      GGL::pixel(y0, x0, color);
    }
    else
    {
      GGL::pixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}
void GGL::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  GGL::drawFastHLine(x, y, w, color);
  GGL::drawFastHLine(x, y + h - 1, w, color);
  GGL::drawFastVLine(x, y, h, color);
  GGL::drawFastVLine(x + w - 1, y, h, color);
}
void GGL::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Update in subclasses if desired!
  GGL::drawLine(x, y, x, y + h - 1, color);
}
void GGL::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Update in subclasses if desired!
  GGL::drawLine(x, y, x + w - 1, y, color);
}
void GGL::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  GGL::drawFastHLine(x + r, y, w - 2 * r, color);         // Top
  GGL::drawFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
  GGL::drawFastVLine(x, y + r, h - 2 * r, color);         // Left
  GGL::drawFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  GGL::drawCircleHelper(x + r, y + r, r, 1, color);
  GGL::drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  GGL::drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  GGL::drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}
void GGL::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  GGL::drawLine(x0, y0, x1, y1, color);
  GGL::drawLine(x1, y1, x2, y2, color);
  GGL::drawLine(x2, y2, x0, y0, color);
}

void GGL::drawFillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  GGL::drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  GGL::drawFillCircleHelper(x0, y0, r, 3, 0, color);
}
void GGL::drawFillCircleHelper(int16_t x0, int16_t y0, int16_t r, int cornername, int16_t delta, uint16_t color)
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
      GGL::drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
      GGL::drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
    }
    if (cornername & 0x2)
    {
      GGL::drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
      GGL::drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
    }
  }
}
void GGL::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // Update in subclasses if desired!
  for (int16_t i = x; i < x + w; i++)
  {
    GGL::drawFastVLine(i, y, h, color);
  }
}
void GGL::drawFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  GGL::drawFillRect(x + r, y, w - 2 * r, h, color);

  // draw four corners
  GGL::drawFillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  GGL::drawFillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}
void GGL::drawFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
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
    GGL::drawFastHLine(a, y0, b - a + 1, color);
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
    GGL::drawFastHLine(a, y, b - a + 1, color);
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
    GGL::drawFastHLine(a, y, b - a + 1, color);
  }
}


/* copies of methods from the u8g2 library */
/* clear buffer */
void GGL::clearBuffer() //+
{
  size_t cnt;
  cnt = _WIDTH;
  cnt *= 20;
  cnt *= 8;
  memset(_LCD_BUFFER, 0, cnt);
}
/* draw bmp image-c-style, 4-gray-color */
void GGL::drawGrayBMP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap)
{
  int i, j, k;
  int page = h * 2 / 8;
  int16_t yy = y * 2; 

  for (k = 0; k < page; k++)
  {
    for (j = 0; j < w; j++)
    {
      for (i = 0; i < 8; i++)
      {
        if (pgm_read_byte(bitmap + j + k * w) & (0x01 << (i & 7)))
        {
          GGL::pixelGray(x + j, yy + i + k * 8, 1);
        }
      }
    }
  }
}
/* draw xbmp image-c-style, 1-bit-color */
void GGL::drawXBMP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap)
{
{
  int i, j, byteWidth = (w + 7) / 8;
  for (j = 0; j < h; j++)
  {
    for (i = 0; i < w; i++)
    {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))
      {
        GGL::pixel(x + i, y + j, 1);
      }
      else
        GGL::pixel(x + i, y + j, 0);
    }
  }
}
}
/* */


/*
void demo_game(int *buffer)
{
  uint16_t cs = 200;
  int r = 5;          // Radius
  int i = 3;          // Stepping variables
  int x = 5, y = 5;   // Starting position
  int dx = i, dy = i; // Stepping
  int x1, y1;         // Save the last position

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

void testLines(int *buffer)
{
  int x1, y1, x2, y2, w = WIDTH, h = HEIGHT;
  int color = 1;
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

void testFilledCircles(int radius, int *buffer)
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

void testCircles(int radius, int *buffer)
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
*/