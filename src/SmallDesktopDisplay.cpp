/* *****************************************************************
  SmallDesktopDisplay 小型桌面显示器

  修改自 https://github.com/chuxin520922/SmallDesktopDisplay/blob/main/2.Firmware/源代码/1.4.2 SD3 PLUS优化/
 * *****************************************************************/

#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <TJpg_Decoder.h>
#include <EEPROM.h>
#include "qr.h"
#include "number.h"
#include "weathernum.h"

#include "SmallDesktopDisplay.h"

SmallDesktopDisplay::SmallDesktopDisplay()
{
}
SmallDesktopDisplay::~SmallDesktopDisplay()
{
}

TFT_eSPI tft = TFT_eSPI();
Adafruit_FT6206 ts = Adafruit_FT6206();

void SmallDesktopDisplay::setBrightness(uint8_t percentage)
{
  if (percentage > 100)
    percentage = 100;
  else if (percentage < 0)
    percentage = 0;

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, int(0xff * percentage / 100));
}

void SmallDesktopDisplay::init(uint8_t rotation)
{
  // setup LCD
  tft.begin();
  tft.setRotation(rotation);
  tft.fillScreen(TFT_BLACK);
  setBrightness();

  // setup touch screen
  Wire.begin(18, 19);

  if (!ts.begin(40))
    Serial.println("Touch screen not working");
}

TS_Point getTouchPoint(uint8_t n = 0)
{
  TS_Point p = ts.getPoint(n);
  p.x = map(p.x, 0, TFT_WIDTH, TFT_WIDTH, 0);
  p.y = map(p.y, 0, TFT_HEIGHT, TFT_HEIGHT, 0);
  uint8_t rotation = tft.getRotation();
  uint16_t width = tft.width();
  uint16_t height = tft.height();
  uint16_t x;
  uint16_t y;

  switch (rotation)
  {
  case 1:
    x = p.y;
    y = height - p.x;
    break;
  case 2:
    x = width - p.x;
    y = height - p.y;
    break;
  case 3:
    x = height - p.y;
    y = p.x;
    break;
  default:
    x = p.x;
    y = p.y;
    break;
  }

  Serial.printf("x: %d, y: %d", x, y);
  Serial.println();
  return TS_Point(x, y, p.z);
}