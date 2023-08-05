#include "WString.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <FastLED.h>
#include <FS.h>

ESP8266WebServer* webServer;
DNSServer dnsServer;
IPAddress local_IP(192, 168, 1, 1);
const char* AP_SSID = "Lamp_Contorl";
const char* AP_PASSWORD = "password";

CRGB leds[128];
/* config JSON format
* {
*   mode: " / ap / lan",
*   wifi: {
*     ssid: "",
*     pwd: ""
*   },
*   led: {
*     power: "on / off",
*     num: 30,
*     brightness: 255,
*     same: "true/false",
*     color: [0xFFFFFF, 0xFFFFFF ,0x...]/0xFFFFFF
*   }
*  }                                                */
DynamicJsonDocument config(2048);

void handleSetup();
void handleLedit();
void changeMode();
void saveConfig();
void initSetupMode();
void initLanMode();
void initApMode();
void initSetup();
void initDashboard();
void initAP();
void initConfig();
void mainRouter();
void mainJSRouter();
void mainCSSRouter();
void setupRouter();
void setupJSRouter();
void setupCSSRouter();

void setupRouter() {
  File file = SPIFFS.open("/setup.html", "r");
  webServer->streamFile(file, "text/html");
  file.close();
}
void setupCSSRouter() {
  File file = SPIFFS.open("/setup.css", "r");
  webServer->streamFile(file, "text/css");
  file.close();
}
void setupJSRouter() {
  File file = SPIFFS.open("/setup.js", "r");
  webServer->streamFile(file, "text/javascript");
  file.close();
}
void mainRouter() {
  File file = SPIFFS.open("/index.html", "r");
  webServer->streamFile(file, "text/html");
  file.close();
}
void mainCSSRouter() {
  File file = SPIFFS.open("/index.css", "r");
  webServer->streamFile(file, "text/css");
  file.close();
}
void mainJSRouter() {
  File file = SPIFFS.open("/index.js", "r");
  webServer->streamFile(file, "text/javascript");
  file.close();
}
// {mode:, wifi:{ssid:, pwd:}, led:{power:off, num:, brightness:255, same:true, color:0xFFFFFF}}
void initConfig() {
  config["mode"] = "";
  config["wifi"]["ssid"] = "";
  config["wifi"]["pwd"] = "";
  config["led"]["power"] = "off";
  config["led"]["num"] = 30;
  config["led"]["brightness"] = 255;
  config["led"]["same"] = "true";
  config["led"]["color"] = 0xFFFFFF;
}
// turn wifimode to ap and open dnsserver
void initAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0));
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    digitalWrite(LED_BUILTIN, LOW);
    while (1)
      ;
  }
  if (!dnsServer.start(53, "*", local_IP)) {
    digitalWrite(LED_BUILTIN, LOW);
    while (1)
      ;
  }
}
void initDashboard() {
  delete webServer;
  webServer = new ESP8266WebServer(80);
  webServer->on("/", mainRouter);
  webServer->on("/index.html", mainRouter);
  webServer->on("/index.css", mainCSSRouter);
  webServer->on("/index.js", mainJSRouter);
  webServer->on("/setup", handleSetup);
  webServer->on("/ledit", handleLedit);
  webServer->onNotFound(mainRouter);
  webServer->begin();
}
void initSetup() {
  delete webServer;
  webServer = new ESP8266WebServer(80);
  webServer->on("/", setupRouter);
  webServer->on("/setup.html", setupRouter);
  webServer->on("/setup.css", setupCSSRouter);
  webServer->on("/setup.js", setupJSRouter);
  webServer->on("/setup", handleSetup);
  webServer->onNotFound(setupRouter);
  webServer->begin();
}
void initApMode() {
  initAP();
  initDashboard();
}
void initLanMode() {
  WiFi.mode(WIFI_STA);
  const char* ssid = config["wifi"]["ssid"];
  const char* pwd = config["wifi"]["pwd"];
  WiFi.begin(ssid, pwd);
  initDashboard();
}
void initSetupMode() {
  initAP();
  initSetup();
}
// save config json to spiffs file
void saveConfig() {
  File config_file = SPIFFS.open("/config.json", "w");
  serializeJson(config, config_file);
  config_file.close();
}
void changeMode() {
  WiFi.disconnect(true);
  if (webServer) {
    webServer->close();
    webServer->stop();
  }
  if (config["mode"] == "ap") {
    initApMode();
  } else if (config["mode"] == "lan") {
    initLanMode();
  } else {
    initSetupMode();
  }
}
// handle POST json
void handleSetup() {
  if (webServer->hasArg("plain") == false) {
    webServer->send(200, "test/plain", "none");
    return;
  } else {
    String mode_old = config["mode"];
    deserializeJson(config, webServer->arg("plain"));
    if (!String(config["mode"]).equals(mode_old)) {
      webServer->send(200, "test/plain", "success");
      saveConfig();
      changeMode();
    }
    webServer->send(200, "test/plain", "success");
  }
}
void handleLedit() {
  if (webServer->hasArg("plain") == false) {
    webServer->send(200, "test/plain", "none");
    return;
  } else {
    deserializeJson(config, webServer->arg("plain"));
    webServer->send(200, "test/plain", "success");
    saveConfig();
    int brightness = config["led"]["brightness"];
    FastLED.setBrightness(brightness);
    int num = config["led"]["num"];
    leds[0] = CRGB::White;
    FastLED.show();
    if (String(config["led"]["power"]).equals("off")) {
      fill_solid(leds, num, CRGB::Black);
    } else if (String(config["led"]["same"]).equals("true")) {
      fill_solid(leds, 30, CRGB::White);
    } else {
      fill_solid(leds, 30, CRGB::White);
    }
    FastLED.show();
  }
}
