#include "ggl.h"

int _WIDTH = 256;
int _HEIGHT = 160;
int _DISPLAY_ROTATE;

int _LCD_BUFFER[256 * 160 / 4]; // 10240

// Command
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

// Settings
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
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); // 8000000

  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RST, HIGH);
  delay(10);
  digitalWrite(LCD_RST, LOW);
  delay(10);
  digitalWrite(LCD_RST, HIGH);
  delay(10);

  transferCommand(0x30); // Extension Command 0
  transferCommand(0x94); // Sleep Out
  delay(50);

  transferCommand(0x31); // Extension Command 1
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
  transferData(0xFF); // xe 256

  transferData(0xA6);

  transferCommand(0x30); // Extension Command 0
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
  transferData(0x9f); // duty 160
  transferData(0x00);

  transferCommand(0xBC); // Data Scan Direction
  transferData(0x00);    // MY=0

  transferCommand(0xaf); // Display On

  digitalWrite(LCD_BL, 0x01);
  // digitalWrite(LCD_BL, 1);
}
void GGL::display()
{
  int page, i;

  transferCommand(0xf0); // Display Mode
  transferData(0x10);    // Monochrome Mode

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
  for (i = 0; i <= _WIDTH * (_HEIGHT / 4); i++)
  {
    _LCD_BUFFER[i] = 0;
  }
}

// Fonts
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

// Monochrome-mode
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

// Gray-mode
void GGL::pixelGray(int x, int y, char color)
{
  GGL::rotate(ROTATE_0);                            // Поворачиваем экран на 0 градусов
  if (x > 256 || y > 256 * 2)                       // Если координаты пикселя находятся за пределами экрана
    return;                                         // Выходим из функции
  if (color)                                        // Если цвет пикселя не равен 0
    _LCD_BUFFER[x + (y / 8) * 256] |= 1 << (y % 8); // Устанавливаем бит в буфере экрана
  else
    _LCD_BUFFER[x + (y / 8) * 256] &= ~(1 << (y % 8)); // Сбрасываем бит в буфере экрана
}
void GGL::bitmapGray(int x, int y, const uint8_t *pBmp, int chWidth, int chHeight)
{
  int i, j, k;
  int16_t yy = y * 2;          // Удваиваем координату y
  int page = chHeight * 2 / 8; // Вычисляем количество страниц
  if (page == 0)               // Если количество страниц равно 0
    page = 1;                  // Устанавливаем количество страниц равным 1

  for (k = 0; k < page; k++) // Цикл по страницам
  {
    for (j = 0; j < chWidth; j++) // Цикл по ширине символа
    {
      for (i = 0; i < 8; i++) // Цикл по высоте символа
      {
        /*  */
        if (pgm_read_byte(pBmp + j + k * chWidth) & (0x01 << (i & 7))) // Если бит в битовой карте равен 1
        {
          GGL::pixelGray(x + j, yy + i + k * 8, 1); // Рисуем пиксель
        }
      }
    }
  }
}
void GGL::displayGray()
{
  int page, i;

  transferCommand(0xf0); // Display Mode
  transferData(0x11);    // 4Gray  Mode

  transferCommand(0x15); // Clumn Address setting
  transferData(0x00);    // XS = 0
  transferData(0xff);    // XE = 256
  transferCommand(0x75); // Page Adress setting
  transferData(0x00);    // XS = 0
  transferData(0x28);    // XE = 159 [0x28]
  transferCommand(0x5c); // ?

  for (page = 0; page <= 40; page++) // 160/4 = 40
  {
    for (i = 0; i < 256; i++)
    {
      transferData(_LCD_BUFFER[i + (page * 256)]);
    }
  }
}
void GGL::sendGrayBuffer()
{
  int page, i;
  for (page = 0; page < 160 / 4; page++)
  {
    for (i = 0; i < 256; i++)
    {
      transferData(_LCD_BUFFER[i + (page * 256)]);
    }
  }
}
void GGL::writeGrayString(int x, int y, const char *pString, int Size, int Mode, Color color)
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

    GGL::writeGrayChar(x, yy, *pString, Size, Mode, color);
    x += Size / 2;
    pString++;
  }
}
void GGL::writeGrayString(int x, int y, const String &text, int Size, int Mode, Color color)
{
  int yy = y * 2; // Учитываем двойную высоту для отрисовки

  // Перебор каждого символа в строке
  for (int i = 0; i < text.length(); i++)
  {
    // // Проверка на выход за пределы экрана по ширине
    // if (x > (_WIDTH - Size / 2))
    // {
    //   x = 0; // Переход на новую строку
    //   yy += Size; // Увеличение вертикальной позиции
    //   if (yy > (_HEIGHT - Size)) // Проверка на выход за пределы экрана по высоте
    //   {
    //     yy = x = 0; // Сброс позиции, если достигнут конец экрана
    //   }
    // }

    // Отрисовка текущего символа
    GGL::writeGrayChar(x, yy, text.charAt(i), Size, Mode, color);
    x += Size / 2; // Сдвиг позиции для следующего символа
  }
}
void GGL::writeGrayChar(unsigned char x, unsigned char y, char acsii, char size, char mode, Color color)
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

    for (j = 0; j < 8; j++) // Цикл, который выполняется 8 раз
    {
      // Обработка битов в зависимости от цвета
      switch (color)
      {
      case BLACK:
        if (temp & 0x80)
        {
          GGL::pixelGray(x, y, 1);     // Первый бит: 1
          GGL::pixelGray(x, y + 1, 1); // Второй бит: 1
          break;
        }
        else
        {
          // GGL::pixelGray(x, y, 0);     // Первый бит: 0
          // GGL::pixelGray(x, y + 1, 0); // Второй бит: 0
          break;
        }

      case DARK_GRAY:
        if (temp & 0x80)
        {
          GGL::pixelGray(x, y, 0);     // Первый бит: 0
          GGL::pixelGray(x, y + 1, 1); // Второй бит: 1
          break;
        }
        else
        {
          // GGL::pixelGray(x, y, 0);     // Первый бит: 0
          // GGL::pixelGray(x, y + 1, 0); // Второй бит: 0
          break;
        }
      case LIGHT_GRAY:
        if (temp & 0x80)
        {
          GGL::pixelGray(x, y, 1);     // Первый бит: 1
          GGL::pixelGray(x, y + 1, 0); // Второй бит: 0
          break;
        }
        else
        {
          // GGL::pixelGray(x, y, 0);     // Первый бит: 0
          // GGL::pixelGray(x, y + 1, 0); // Второй бит: 0
          break;
        }
      case WHITE:
        if (temp & 0x80)
        {
          GGL::pixelGray(x, y, 0);     // Первый бит: 0
          GGL::pixelGray(x, y + 1, 0); // Второй бит: 0
          break;
        }
        else
        {
          // GGL::pixelGray(x, y, 0);     // Первый бит: 0
          // GGL::pixelGray(x, y + 1, 0); // Второй бит: 0
          break;
        }
      }

      temp <<= 1; // Сдвигаем temp на один бит влево

      y += 2; // Переходим к следующему пикселю вниз (пропускаем строку)

      if ((y - y0) >= size * 2) // Если мы достигли конца строки (учитываем удвоение строк)
      {
        y = y0; // Возвращаемся к началу строки
        x++;    // Переходим к следующему столбцу
        break;  // Выходим из цикла
      }
    }
  }
}

// Draw gray-mode
void GGL::drawGrayPixel(int x, int y, Color color)
{
  short yy = y * 2; 
  GGL::rotate(ROTATE_0);
  if (x > 256 || yy > 256 * 2)
    return;

  switch (color)
  {
  case BLACK:
    GGL::pixelGray(x, yy, 1);     // Первый бит: 0
    GGL::pixelGray(x, yy + 1, 1); // Второй бит: 0
    break;
  case DARK_GRAY:
    GGL::pixelGray(x, yy, 0);     // Первый бит: 0
    GGL::pixelGray(x, yy + 1, 1); // Второй бит: 1
    break;
  case LIGHT_GRAY:
    GGL::pixelGray(x, yy, 1);     // Первый бит: 0
    GGL::pixelGray(x, yy + 1, 0); // Второй бит: 0
    break;
  case WHITE:
    GGL::pixelGray(x, yy, 0);     // Первый бит: 0
    GGL::pixelGray(x, yy + 1, 0); // Второй бит: 0
    break;
  }
}
void GGL::drawGrayFillFrame(int x, int y, int w, int h, Color borderColor, Color fillColor)
{
  // Проверка на корректность размеров
  if (w <= 0 || h <= 0)
    return;

  // Отрисовка верхней границы
  for (int i = x; i < x + w; i++)
  {
    GGL::drawGrayPixel(i, y, borderColor); // Верхняя граница
  }

  // Отрисовка нижней границы
  for (int i = x; i < x + w; i++)
  {
    GGL::drawGrayPixel(i, y + h - 1, borderColor); // Нижняя граница
  }

  // Отрисовка левой границы
  for (int j = y; j < y + h; j++)
  {
    GGL::drawGrayPixel(x, j, borderColor); // Левая граница
  }

  // Отрисовка правой границы
  for (int j = y; j < y + h; j++)
  {
    GGL::drawGrayPixel(x + w - 1, j, borderColor); // Правая граница
  }

  // Заполнение внутренней области (если требуется)
  // if (fillColor != WHITE) // Если цвет заполнения не белый
  // {
    for (int i = x + 1; i < x + w - 1; i++) // Внутренняя область по ширине
    {
      for (int j = y + 1; j < y + h - 1; j++) // Внутренняя область по высоте
      {
        GGL::drawGrayPixel(i, j, fillColor); // Заполнение внутренней области
      }
    }
  // }
}
void GGL::drawGrayFrame(int x, int y, int w, int h, Color borderColor)
{
  // Проверка на корректность размеров
  if (w <= 0 || h <= 0)
    return;

  // Отрисовка верхней границы
  for (int i = x; i < x + w; i++)
  {
    GGL::drawGrayPixel(i, y, borderColor); // Верхняя граница
  }

  // Отрисовка нижней границы
  for (int i = x; i < x + w; i++)
  {
    GGL::drawGrayPixel(i, y + h - 1, borderColor); // Нижняя граница
  }

  // Отрисовка левой границы
  for (int j = y; j < y + h; j++)
  {
    GGL::drawGrayPixel(x, j, borderColor); // Левая граница
  }

  // Отрисовка правой границы
  for (int j = y; j < y + h; j++)
  {
    GGL::drawGrayPixel(x + w - 1, j, borderColor); // Правая граница
  }
}
void GGL::drawGrayRoundedFrame(int x, int y, int w, int h, int r, Color borderColor, Color fillColor)
{
  // Проверка на корректность размеров
  if (w <= 0 || h <= 0 || r <= 0)
    return;

  // Ограничение радиуса, чтобы он не превышал половину ширины или высоты
  r = min(r, min(w / 2, h / 2));

  // Отрисовка верхней границы
  for (int i = x + r; i < x + w - r; i++)
  {
    GGL::drawGrayPixel(i, y, borderColor); // Верхняя граница
  }

  // Отрисовка нижней границы
  for (int i = x + r; i < x + w - r; i++)
  {
    GGL::drawGrayPixel(i, y + h - 1, borderColor); // Нижняя граница
  }

  // Отрисовка левой границы
  for (int j = y + r; j < y + h - r; j++)
  {
    GGL::drawGrayPixel(x, j, borderColor); // Левая граница
  }

  // Отрисовка правой границы
  for (int j = y + r; j < y + h - r; j++)
  {
    GGL::drawGrayPixel(x + w - 1, j, borderColor); // Правая граница
  }

  // Отрисовка закругленных углов
  for (int i = 0; i <= r; i++)
  {
    for (int j = 0; j <= r; j++)
    {
      if (i * i + j * j <= r * r) // Проверка, находится ли точка внутри окружности
      {
        // Левый верхний угол
        GGL::drawGrayPixel(x + r - i, y + r - j, borderColor);
        // Правый верхний угол
        GGL::drawGrayPixel(x + w - r + i - 1, y + r - j, borderColor);
        // Левый нижний угол
        GGL::drawGrayPixel(x + r - i, y + h - r + j - 1, borderColor);
        // Правый нижний угол
        GGL::drawGrayPixel(x + w - r + i - 1, y + h - r + j - 1, borderColor);
      }
    }
  }

  // Заполнение внутренней области (если требуется)
  if (fillColor != WHITE) // Если цвет заполнения не белый
  {
    for (int i = x + 1; i < x + w - 1; i++) // Внутренняя область по ширине
    {
      for (int j = y + 1; j < y + h - 1; j++) // Внутренняя область по высоте
      {
        // Проверка, чтобы не заходить в закругленные углы
        if (!((i < x + r && j < y + r) || // Левый верхний угол
              (i >= x + w - r && j < y + r) || // Правый верхний угол
              (i < x + r && j >= y + h - r) || // Левый нижний угол
              (i >= x + w - r && j >= y + h - r))) // Правый нижний угол
        {
          GGL::drawGrayPixel(i, j, fillColor); // Заполнение внутренней области
        }
      }
    }
  }
}
void GGL::drawGrayFillCircle(int x0, int y0, int r, Color borderColor, Color fillColor)
{
  int x = 0; // Текущая координата x
  int y = r; // Текущая координата y
  int d = 3 - 2 * r; // Начальное значение решения

  // Заполнение окружности, если fillColor не равен WHITE
  if (fillColor != WHITE)
  {
    for (int dy = -r; dy <= r; dy++) // Перебор по вертикали
    {
      for (int dx = -r; dx <= r; dx++) // Перебор по горизонтали
      {
        // Проверка, находится ли точка внутри окружности
        if (dx * dx + dy * dy <= r * r)
        {
          GGL::drawGrayPixel(x0 + dx, y0 + dy, fillColor); // Заполнение внутренней области
        }
      }
    }
  }

  // Отрисовка окружности по алгоритму Брезенхема
  while (x <= y)
  {
    // Отрисовка 8 симметричных точек окружности
    GGL::drawGrayPixel(x0 + x, y0 + y, borderColor); // Октант 1
    GGL::drawGrayPixel(x0 + y, y0 + x, borderColor); // Октант 2
    GGL::drawGrayPixel(x0 - x, y0 + y, borderColor); // Октант 4
    GGL::drawGrayPixel(x0 - y, y0 + x, borderColor); // Октант 3
    GGL::drawGrayPixel(x0 + x, y0 - y, borderColor); // Октант 8
    GGL::drawGrayPixel(x0 + y, y0 - x, borderColor); // Октант 7
    GGL::drawGrayPixel(x0 - x, y0 - y, borderColor); // Октант 5
    GGL::drawGrayPixel(x0 - y, y0 - x, borderColor); // Октант 6

    // Обновление решения и координат
    if (d < 0)
    {
      d = d + 4 * x + 6;
    }
    else
    {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}
void GGL::drawGrayCircle(int x0, int y0, int r, Color color)
{
  int x = 0; // Текущая координата x
  int y = r; // Текущая координата y
  int d = 3 - 2 * r; // Начальное значение решения

  // Отрисовка окружности по алгоритму Брезенхема
  while (x <= y)
  {
    // Отрисовка 8 симметричных точек окружности
    GGL::drawGrayPixel(x0 + x, y0 + y, color); // Октант 1
    GGL::drawGrayPixel(x0 + y, y0 + x, color); // Октант 2
    GGL::drawGrayPixel(x0 - x, y0 + y, color); // Октант 4
    GGL::drawGrayPixel(x0 - y, y0 + x, color); // Октант 3
    GGL::drawGrayPixel(x0 + x, y0 - y, color); // Октант 8
    GGL::drawGrayPixel(x0 + y, y0 - x, color); // Октант 7
    GGL::drawGrayPixel(x0 - x, y0 - y, color); // Октант 5
    GGL::drawGrayPixel(x0 - y, y0 - x, color); // Октант 6

    // Обновление решения и координат
    if (d < 0)
    {
      d = d + 4 * x + 6;
    }
    else
    {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}
void GGL::drawGrayTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color borderColor, Color fillColor)
{
  // Функция для отрисовки линии между двумя точками
  auto drawLine = [&](int xStart, int yStart, int xEnd, int yEnd, Color color) {
    int dx = abs(xEnd - xStart);
    int dy = abs(yEnd - yStart);
    int sx = (xStart < xEnd) ? 1 : -1;
    int sy = (yStart < yEnd) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
      GGL::drawGrayPixel(xStart, yStart, color); // Отрисовка пикселя

      if (xStart == xEnd && yStart == yEnd) // Если достигли конечной точки
        break;

      int e2 = 2 * err;
      if (e2 > -dy)
      {
        err -= dy;
        xStart += sx;
      }
      if (e2 < dx)
      {
        err += dx;
        yStart += sy;
      }
    }
  };

  // Отрисовка границ треугольника
  drawLine(x0, y0, x1, y1, borderColor); // Линия между (x0, y0) и (x1, y1)
  drawLine(x1, y1, x2, y2, borderColor); // Линия между (x1, y1) и (x2, y2)
  drawLine(x2, y2, x0, y0, borderColor); // Линия между (x2, y2) и (x0, y0)

  // Заполнение треугольника, если fillColor не равен WHITE
  if (fillColor != WHITE)
  {
    // Находим bounding box треугольника
    int minX = min(x0, min(x1, x2));
    int maxX = max(x0, max(x1, x2));
    int minY = min(y0, min(y1, y2));
    int maxY = max(y0, max(y1, y2));

    // Функция для проверки, находится ли точка внутри треугольника
    auto pointInTriangle = [&](int x, int y) {
      auto sign = [](int x1, int y1, int x2, int y2, int x3, int y3) {
        return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
      };

      float d1 = sign(x, y, x0, y0, x1, y1);
      float d2 = sign(x, y, x1, y1, x2, y2);
      float d3 = sign(x, y, x2, y2, x0, y0);

      bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
      bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

      return !(hasNeg && hasPos);
    };

    // Заполнение треугольника
    for (int y = minY; y <= maxY; y++) // Перебор по вертикали
    {
      for (int x = minX; x <= maxX; x++) // Перебор по горизонтали
      {
        if (pointInTriangle(x, y)) // Если точка внутри треугольника
        {
          GGL::drawGrayPixel(x, y, fillColor); // Заполнение внутренней области
        }
      }
    }
  }
}
void GGL::drawGrayHLine(int x, int y, int length, Color color, int w = 1)
{
  // Проверка на корректность длины и ширины
  if (length <= 0 || w <= 0)
    return;

  // Отрисовка линии
  for (int i = 0; i < w; i++) // Цикл по ширине линии
  {
    for (int j = 0; j < length; j++) // Цикл по длине линии
    {
      GGL::drawGrayPixel(x + j, y + i, color); // Отрисовка пикселя
    }
  }
}
void GGL::drawGrayVLine(int x, int y, int length, Color color, int w = 1)
{
  // Проверка на корректность длины и ширины
  if (length <= 0 || w <= 0)
    return;

  // Отрисовка линии
  for (int i = 0; i < w; i++) // Цикл по ширине линии
  {
    for (int j = 0; j < length; j++) // Цикл по длине линии
    {
      GGL::drawGrayPixel(x + i, y + j, color); // Отрисовка пикселя
    }
  }
}
void GGL::drawGrayBox(int x, int y, int w, int h, Color fillColor)
{
  // Проверка на корректность размеров
  if (w <= 0 || h <= 0)
    return;

  // Отрисовка прямоугольника
  for (int i = 0; i < w; i++) // Цикл по ширине
  {
    for (int j = 0; j < h; j++) // Цикл по высоте
    {
      GGL::drawGrayPixel(x + i, y + j, fillColor); // Отрисовка пикселя
    }
  }
}
void GGL::drawGrayLine(int x0, int y0, int x1, int y1, Color color)
{
  // Вычисление разницы координат
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  // Определение направления шага
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;

  // Ошибка (разница между текущей и идеальной линией)
  int err = dx - dy;

  // Основной цикл отрисовки линии
  while (true)
  {
    // Отрисовка текущего пикселя
    GGL::drawGrayPixel(x0, y0, color);

    // Если достигли конечной точки, завершаем цикл
    if (x0 == x1 && y0 == y1)
      break;

    // Вычисление следующего шага
    int e2 = 2 * err;
    if (e2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}
void GGL::drawGraySine(uint16_t y, uint16_t a, uint16_t n, Color color)
{
  uint16_t x1 = 0, x2;
  uint16_t y1 = _HEIGHT / 2, y2;
  for (x2 = 0; x2 < _WIDTH; x2++)
  {
    y2 = y + (a * sin(0.0175 * n * x2));
    GGL::drawGrayLine(x1, y1, x2, y2, color);
    x1 = x2;
    y1 = y2;
  }
}

// Draw Table
void GGL::drawGrayTable(int x, int y, int rows, int cols, int cellWidth, int cellHeight, const char* data, Color borderColor, Color textColor, Color bgColor)
{
  // Проверка на корректность входных данных
  if (rows <= 0 || cols <= 0 || cellWidth <= 0 || cellHeight <= 0 || data == nullptr)
    return;

  // Создаем копию входной строки для разделения
  char buffer[256]; // Буфер для копии строки
  strncpy(buffer, data, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0'; // Гарантируем завершение строки

  // Разделяем строку на значения
  char* token = strtok(buffer, ","); // Первое значение
  int index = 0; // Индекс текущего значения

  // Отрисовка таблицы
  for (int i = 0; i < rows; i++) // Цикл по строкам
  {
    for (int j = 0; j < cols; j++) // Цикл по столбцам
    {
      // Координаты текущей ячейки
      int cellX = x + j * cellWidth;
      int cellY = y + i * cellHeight;
      // int shiftX = (cellWidth/2) - 5; // Смещение символа в ячейке

      // Отрисовка границ ячейки
      GGL::drawGrayBox(cellX, cellY, cellWidth, cellHeight, borderColor);

      // Заполнение ячейки цветом фона
      GGL::drawGrayBox(cellX + 1, cellY + 1, cellWidth - 2, cellHeight - 2, bgColor);

      // Если есть данные для текущей ячейки
      if (token != nullptr && index < rows * cols)
      {
        // Отрисовка текста в ячейке
        int len = strlen(token);
        for (int k = 0; k < len; k++)
        {
          GGL::writeGrayChar(cellX + 2 + k * 8, (cellY + 2) * 2, token[k], 12, 1, textColor); // Отрисовка символа
        }

        // Переход к следующему значению
        token = strtok(nullptr, ",");
        index++;
      }
    }
  }
}

// Draw mochrome-mode
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
  // int page, i;
  // for (page = 0; page < 41; page++) // 160/4 = 40
  // {
  //   for (i = 0; i < 256; i++)
  //   {
  //     transferData(0x00);
  //   }
  // }
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
/*
void drawBox(int x, int y, int w, int h);
void drawCircle(int x0, int y0, int rad);
void drawDisc(int x0, int y0, int rad);
void drawEllipse(int x0, int y0, int rx, int ry);
void drawFilledEllipse(int x0, int y0, int rx, int ry);
void drawFrame(int x, int y, int w, int h);
void drawHLine(int x, int y, int w);
void drawLine(int x0, int y0, int x1, int y1);
void drawPixel(int x, int y);
void drawRBox(int x, int y, int w, int h, int r);
void drawRFrame(int x, int y, int w, int h, int r);
uint8_t drawStr(int x, int y, const char *s);
uint8_t drawStrX2(int x, int y, const char *s);
void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
void drawVLine(int x, int y, int h);
void setCursor(int x, int y);
void setContrast(int value);*/
