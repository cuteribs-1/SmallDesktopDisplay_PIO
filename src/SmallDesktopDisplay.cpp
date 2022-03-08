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

SmallDesktopDisplay::SmallDesktopDisplay(uint8_t rotation)
{
  _rotation = rotation;
}
SmallDesktopDisplay::~SmallDesktopDisplay()
{
}

void SmallDesktopDisplay::setBrightness(uint8_t percentage)
{
  if (percentage > 100)
    percentage = 100;
  else if (percentage < 0)
    percentage = 0;

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, int(0xff * percentage / 100));
}

void SmallDesktopDisplay::init()
{
  // setup LCM
  Serial.println("Setup LCM");
  tft.begin();
  tft.setRotation(_rotation);
  tft.fillScreen(TFT_BLACK);
  setBrightness();

  // setup touch screen
  Serial.println("Setup TP");
  Wire.begin(18, 19);
  Serial.printf("Touch screen: %d", ts.begin(40));
  Serial.println();
}

uint8_t SmallDesktopDisplay::touched()
{
  return ts.touched();
}

TS_Point SmallDesktopDisplay::getTouchPoint(uint8_t n)
{
  TS_Point p = ts.getPoint(n);
  p.x = map(p.x, 0, TFT_WIDTH, TFT_WIDTH, 0);
  p.y = map(p.y, 0, TFT_HEIGHT, TFT_HEIGHT, 0);
  uint16_t x, y;

  switch (_rotation)
  {
  case 1:
    x = p.y;
    y = TFT_WIDTH - p.x;
    break;
  case 2:
    x = TFT_WIDTH - p.x;
    y = TFT_HEIGHT - p.y;
    break;
  case 3:
    x = p.y;
    y = TFT_WIDTH - p.x;
    break;
  default:
    x = p.x;
    y = p.y;
    break;
  }

  // Serial.printf("end r: %d, x: %d, y: %d", _rotation, x, y);
  // Serial.println();
  TS_Point point = TS_Point(x, y, p.z);

  if (_lastPoint != point)
    _lastPoint = point;

  return point;
}