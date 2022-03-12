

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WiFi.h>
// #include <SPI.h>
// #include <TJpg_Decoder.h>
// #include <EEPROM.h>
// #include "qr.h"
// #include "number.h"
// #include "weathernum.h"
#include "SmallDesktopDisplay.h"

void SmallDesktopDisplay::syncWeather()
{
  /*
  {
    "status": "1",
    "count": "1",
    "info": "OK",
    "infocode": "10000",
    "lives": [
      {
        "province": "上海",
        "city": "长宁区",
        "adcode": "310105",
        "weather": "晴",
        "temperature": "12",
        "winddirection": "北",
        "windpower": "≤3",
        "humidity": "63",
        "reporttime": "2022-03-10 01:31:45"
      }
    ]
  }
  */

  Serial.println("Start syncing weather");
  String apiUrl = "https://restapi.amap.com/v3/weather/weatherInfo?city=310105&key=8b91ceb0b60c0d46e531444c209543ae";

  if (_http.begin(apiUrl, AMAP_CERT.c_str()))
  {
    if (_http.GET() == HTTP_CODE_OK)
    {
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, _http.getStream());
      JsonObject weather = doc["lives"][0];

      char text[50];
      u16_t textWidth;
      TFT_eSprite *spr;

      sprintf(
          text, "%s %s - %s",
          doc["province"].as<String>(),
          weather["city"].as<String>(),
          weather["weather"].as<String>());
      Serial.println(text);

      spr = new TFT_eSprite(&tft);
      spr->loadFont(FONT_NOTO_SANS_SC, LittleFS);
      textWidth = spr->textWidth(text);
      spr->createSprite(textWidth, 24);
      spr->fillSprite(BACK_COLOR);
      spr->setTextColor(FORE_COLOR, BACK_COLOR);
      spr->print(text);
      spr->pushSprite(30, 10);
      spr->unloadFont();
      delete spr;
    }
  }

  _http.end();
  Serial.println("Complete syncing weather");
}

void SmallDesktopDisplay::syncNTPTime()
{
  Serial.println("Start syncing NTP");
  configTime(0, 0, "ntp1.aliyun.com", "ntp2.aliyun.com", "ntp3.aliyun.com");
  tm ntpTime;

  if (!getLocalTime(&ntpTime))
  {
    Serial.println("Failed to get time from NTP");
    delay(1000);
    syncNTPTime();
    return;
  }
  else
  {
    Serial.printf("%04d-%02d-%02d %02d:%02d:%02d %d", ntpTime.tm_year + 1900, ntpTime.tm_mon, ntpTime.tm_mday, ntpTime.tm_hour, ntpTime.tm_min, ntpTime.tm_sec, ntpTime.tm_isdst);
    Serial.println();

    setTime(mktime(&ntpTime));
    adjustTime(8 * 3600);
  }

  Serial.println("Complete syncing NTP");
}

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
  setBrightness();
  tft.fillScreen(BACK_COLOR);

  // setup font
  if (!LittleFS.begin())
  {
    Serial.println("Flash FS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }

  Serial.println("Flash FS available!");
  char fontFile[30];
  sprintf(fontFile, "/%s.vlw", FONT_NOTO_SANS_SC);

  if (!LittleFS.exists(fontFile))
  {
    Serial.println("Font missing in Flash FS, did you upload it?");
    while (1)
      yield();
  }

  Serial.println("Fonts found OK!");

  // setup touch screen
  Serial.println("Setup TP");
  Wire.begin(18, 19);
  Serial.printf("Touch screen: %d", ts.begin(40));
  Serial.println();
}

void SmallDesktopDisplay::connectWifi()
{
  Serial.println("Start connecting WiFi");

  if (WiFi.status() == WL_CONNECTED)
    return;

  if (_wifiConfig.ssid.isEmpty())
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    Serial.println("Waiting for SmartConfig.");

    while (!WiFi.smartConfigDone())
    {
      delay(300);
      Serial.print(".");
    }

    Serial.println();
    Serial.println("SmartConfig received.");
  }
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiConfig.ssid.c_str(), _wifiConfig.password.c_str());
  }

  Serial.println("WiFi Connecting");
  u32_t startAt = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");

    if (millis() - startAt > 10 * 1000)
    {
      _wifiConfig = {{""}, {""}};
      Serial.println("Failed to connect WiFi, switching to SmartConfig");
      connectWifi();
      return;
    }
  }

  Serial.println();
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void SmallDesktopDisplay::syncInfo()
{
  WiFi.setSleep(WIFI_PS_NONE);
  syncNTPTime();
  syncWeather();
  WiFi.setSleep(WIFI_PS_MAX_MODEM);
}

void SmallDesktopDisplay::renderTime()
{
  time_t t = now();

  if (t == _lastTime)
    return;

  tmElements_t tm;
  breakTime(t, tm);
  char text[30];
  u16_t textWidth;
  TFT_eSprite *spr;

  if (tm.Second != _lastTM.Second || tm.Minute != _lastTM.Minute || tm.Hour != _lastTM.Hour)
  {
    spr = new TFT_eSprite(&tft);
    spr->loadFont(FONT_NOTO_SANS_SC, LittleFS);
    sprintf(text, "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
    textWidth = spr->textWidth(text);
    spr->createSprite(textWidth, 24);
    spr->fillSprite(BACK_COLOR);
    spr->setTextColor(FORE_COLOR, BACK_COLOR);
    spr->setTextDatum(CC_DATUM);
    spr->print(text);
    spr->pushSprite(50, 120);
    spr->unloadFont();
    delete spr;
  }

  if (tm.Day != _lastTM.Day || tm.Month != _lastTM.Month || tm.Year != _lastTM.Year)
  {
    spr = new TFT_eSprite(&tft);
    spr->loadFont(FONT_NOTO_SANS_SC, LittleFS);
    sprintf(text, "%04d%s%02d%s%02d%s", tm.Year + 1970, "年", tm.Month, "月", tm.Day, "日");
    textWidth = spr->textWidth(text);
    spr->createSprite(textWidth, 24);
    spr->fillSprite(BACK_COLOR);
    spr->setTextColor(FORE_COLOR, BACK_COLOR);
    spr->print(text);
    spr->pushSprite(50, 80);
    spr->unloadFont();
    delete spr;
  }

  _lastTime = t;
  _lastTM = tm;
}

u8_t SmallDesktopDisplay::touched()
{
  return ts.touched();
}

TS_Point SmallDesktopDisplay::getTouchPoint(u8_t n)
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
