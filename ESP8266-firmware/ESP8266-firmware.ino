#include "ESP8266-firmware.h"

void setup() {
  initConfig();
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<WS2812, 2, RGB>(leds, 128);
  SPIFFS.begin();
  File config_file = SPIFFS.open("/config.json", "r");
  if (config_file)
    deserializeJson(config, config_file);
  config_file.close();
  changeMode();
}

void loop() {
  webServer->handleClient();
  if (!String(config["mode"]).equals("lan")) {
    dnsServer.processNextRequest();
    if (WiFi.softAPgetStationNum() < 1) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
    }
  }
}