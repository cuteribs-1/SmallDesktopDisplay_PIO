/* *****************************************************************
  SmallDesktopDisplay 小型桌面显示器

  修改自 https://github.com/chuxin520922/SmallDesktopDisplay/blob/main/2.Firmware/源代码/1.4.2 SD3 PLUS优化/
 * *****************************************************************/

#include <TFT_eSPI.h>
#include <Adafruit_FT6206.h>

class SmallDesktopDisplay
{
private:
  uint8_t _rotation;
  TS_Point _lastPoint;

public:
  SmallDesktopDisplay(uint8_t rotation = 0);
  ~SmallDesktopDisplay();
  void init();
  void setBrightness(uint8_t percentage = 50);
  uint8_t touched();
  TS_Point getTouchPoint(uint8_t n = 0);
  TFT_eSPI tft = TFT_eSPI();
  Adafruit_FT6206 ts = Adafruit_FT6206();
};