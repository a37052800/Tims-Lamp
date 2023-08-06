#include "ESP8266-firmware.h"

void setup()
{
  // SPIFFS.format();
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<WS2812, 2, RGB>(leds, 128);
  SPIFFS.begin();
  File config_file = SPIFFS.open("/config.json", "r");
  if (config_file)
    deserializeJson(config, config_file);
  else
    initConfig();
  config_file.close();
  setNetService();
  applyLEDConfig();
  apiService();
}

void loop()
{
  webServer->handleClient();
  dnsServer.processNextRequest();
  if (WiFi.softAPgetStationNum() < 1)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }
  digitalWrite(LED_BUILTIN, HIGH);
}