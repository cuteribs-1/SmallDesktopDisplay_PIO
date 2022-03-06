#include "SmallDesktopDisplay.h"
#include <Update.h>
#include <WebServer.h>
#include <Wire.h>
#include <DNSServer.h>

SmallDesktopDisplay sdd = SmallDesktopDisplay();

void setup()
{
	sdd.init();
	sdd.setBrightness(20);
}

void loop()
{
	if(sdd.touched())
		sdd.getTouchPoint();
}