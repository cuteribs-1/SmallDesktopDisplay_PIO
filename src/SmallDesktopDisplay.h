/* *****************************************************************
  SmallDesktopDisplay 小型桌面显示器

  修改自 https://github.com/chuxin520922/SmallDesktopDisplay/blob/main/2.Firmware/源代码/1.4.2 SD3 PLUS优化/
 * *****************************************************************/

#include <TFT_eSPI.h>
#include <Adafruit_FT6206.h>

class SmallDesktopDisplay
{
private:
  TFT_eSPI tft;
  Adafruit_FT6206 ts;

public:
  SmallDesktopDisplay();
  ~SmallDesktopDisplay();
  void init(uint8_t rotation = 0);
  void setBrightness(uint8_t percentage = 50);
  uint8_t touched();
  TS_Point getTouchPoint(uint8_t n = 0);
};