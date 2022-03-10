#include "SmallDesktopDisplay.h"
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "main.h"
#include <TimeLib.h>
#include <ArduinoJson.h>

SmallDesktopDisplay _sdd(1);
WiFiConfig _wifiConfig = {{"K2P2"}, {"1234123412"}};
HTTPClient _http;
u16_t _syncDelay = 10 * 60;
u16_t _frameDelay = int(1000 / 30);
u32_t _lastMillis;

String cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n"
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n"
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n"
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n"
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n"
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n"
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n"
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n"
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n"
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n"
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n"
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n"
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n"
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n"
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n"
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n"
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n"
    "-----END CERTIFICATE-----\n";

void renderTime()
{
  time_t t = now();
  TFT_eSPI tft = _sdd.tft;
  tft.setCursor(50, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("%04d-%02d-%02d", year(t), month(t), day(t));

  tft.setCursor(50, 120);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("%02d:%02d:%02d", hour(t), minute(t), second(t));
}

void connectWifi()
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

void syncNTPTime()
{
  Serial.println("Start syncing NTP");
  configTime(0, 0, "ntp1.aliyun.com", "ntp2.aliyun.com", "ntp3.aliyun.com");
  tm ntpTime;

  if (!getLocalTime(&ntpTime))
  {
    Serial.println("Failed to get time from NTP");
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

void syncWeather()
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

  if (_http.begin(apiUrl, cert.c_str()))
  {
    if (_http.GET() == HTTP_CODE_OK)
    {
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, _http.getStream());
      JsonObject weather = doc["lives"][0];
      TFT_eSPI tft = _sdd.tft;
      char text[30];
      sprintf(text, "%s %s - %s", weather["province"], weather["city"], weather["weather"]);
      tft.drawString(text, 30, 10, 4);
    }
  }

  _http.end();
  Serial.println("Complete syncing weather");
}

void syncInfo()
{
  WiFi.setSleep(WIFI_PS_NONE);
  syncNTPTime();
  syncWeather();
  WiFi.setSleep(WIFI_PS_MAX_MODEM);
}

void setup()
{
  Serial.begin(115200);
  _sdd.init();
  _sdd.setBrightness(30);
  connectWifi();
  syncInfo();
  _lastMillis = millis();
}

void loop()
{
  u32_t pastMillis = millis() - _lastMillis;

  if (pastMillis >= _syncDelay)
  {
    syncInfo();
  }
  else if (pastMillis >= _frameDelay)
  {
    renderTime();
    _lastMillis = millis();
  }
}
