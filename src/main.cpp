/* *****************************************************************
  SmallDesktopDisplay 小型桌面显示器

  修改自 https://github.com/chuxin520922/SmallDesktopDisplay/blob/main/2.Firmware/源代码/1.4.2 SD3 PLUS优化/
 * *****************************************************************/

#include "SmallDesktopDisplay.h"

SmallDesktopDisplay _sdd(1);
u32_t _syncDelay = 10 * 60 * 1000;
u16_t _frameDelay = int(1000 / 30);
u32_t _lastMillis;

void setup()
{
  Serial.begin(115200);
  _sdd.init();
  _sdd.setBrightness(30);

  _sdd.connectWifi();
  _sdd.syncInfo();

  _lastMillis = millis();
}

void loop()
{
  u32_t pastMillis = millis() - _lastMillis;

  if (pastMillis >= _syncDelay)
  {
    _sdd.syncInfo();
    _lastMillis = millis();
  }
  else if (pastMillis >= _frameDelay)
  {
    _sdd.renderTime();
    _lastMillis = millis();
  }
}
