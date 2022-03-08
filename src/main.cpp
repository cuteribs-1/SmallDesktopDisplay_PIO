#include "SmallDesktopDisplay.h"
#include <Wire.h>
#include <WiFi.h>
#include "main.h"
#include <TimeLib.h>
#include <Ticker.h>

SmallDesktopDisplay _sdd = SmallDesktopDisplay(1);
WiFiConfig _wifiConfig = {{"ASUS"}, {"1234123412"}};
u16_t _frameDelay = int(1000 / 30);
tm _lastSyncTime;

Ticker _tickerNTPSync;
Ticker _tickerOnFrame;

void renderTime()
{
	time_t t = now();
	TFT_eSPI tft = _sdd.tft;
	tft.setCursor(100, 100, 8);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.printf("%04d:%02d:%02d", hour(t), minute(t), second(t));
}

void connectWifi()
{
	Serial.println("Start connecting WiFi");

	if (WiFi.status() == WL_CONNECTED)
		return;

	if (_wifiConfig.ssid.isEmpty() || true)
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

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(300);
		Serial.print(".");
	}

	Serial.println("WiFi Connected.");
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
}

void syncNTPTime()
{
	connectWifi();
	Serial.println("Start syncing NTP");
	configTime(8 * 3600, 0, "ntp1.aliyun.com", "ntp2.aliyun.com", "ntp3.aliyun.com");
	_tickerOnFrame.detach();

	if (!getLocalTime(&_lastSyncTime))
	{
		Serial.println("Failed to get time from NTP");
	}
	else
	{
		_tickerOnFrame.attach_ms(_frameDelay, renderTime);
	}

	_tickerNTPSync.detach();
	_tickerNTPSync.attach(10 * 60, syncNTPTime);
}

void setup()
{
	Serial.begin(115200);
	_sdd.init();
	_sdd.setBrightness(30);

	Serial.println("Setup Tickers");
	_tickerNTPSync.once_ms(0, syncNTPTime);
}

void loop()
{
}
