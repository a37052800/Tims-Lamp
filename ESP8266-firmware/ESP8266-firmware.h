#include "WString.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <FastLED.h>
#include <FS.h>

ESP8266WebServer *webServer;
DNSServer dnsServer;
DynamicJsonDocument config(256);

CRGB leds[128];

void initConfig();
void applyLEDConfig();
void setNetService();
void apiService();
void handleAPI();
void handleNotFound();
void saveConfig();
void error();

void initConfig()
{
  config["mode"] = "ap";
  config["wifi"]["ssid"] = "Lamp_Contorl";
  config["wifi"]["pwd"] = "password";
  config["led"]["power"] = "off";
  config["led"]["num"] = 30;
  config["led"]["brightness"] = 64;
  config["led"]["same"] = "true";
  config["led"]["color"] = "0xFFFFFF";
}

void applyLEDConfig()
{
  int brightness = config["led"]["brightness"];
  FastLED.setBrightness(brightness);
  int num = config["led"]["num"];
  if (config["led"]["power"] == "on")
    if (config["led"]["same"] == "true")
    {
      std::string color = config["led"]["color"];
      fill_solid(leds, num, CRGB::White); //TODO stoi
    }
    else
    {
      // TODO finish muti color
    }
  else
    fill_solid(leds, num, CRGB::Black);
}

void setNetService()
{
  String mode = config["mode"];
  String ssid = config["wifi"]["ssid"];
  String pwd = config["wifi"]["pwd"];
  WiFi.disconnect(true);
  if (webServer)
  {
    webServer->close();
    webServer->stop();
  }
  if (mode == "ap")
  {
    IPAddress local_IP(192, 168, 2, 1);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0));
    if (!WiFi.softAP(ssid, pwd))
      error();
    if (!dnsServer.start(53, "*", local_IP))
      error();
  }
  else if (mode == "lan")
  {
    dnsServer.stop();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
  }
  else
    error();
}

void apiService()
{
  delete webServer;
  webServer = new ESP8266WebServer(80);
  webServer->on("/api", handleAPI);
  webServer->onNotFound(handleNotFound);
  webServer->begin();
}

void handleAPI()
{
  if (webServer->hasArg("plain") == false)
  {
    webServer->send(204, "test/plain", "only for http_post");
    return;
  }
  webServer->send(200, "test/plain", webServer->arg("plain"));
  DynamicJsonDocument _config(256);
  deserializeJson(_config, webServer->arg("plain"));
  bool configChange = 0, netChange = 0;
  if ((!_config["mode"].isNull()) && (_config["mode"] != config["mode"]))
  {
    netChange = 1;
    config["mode"] = _config["mode"];
  }
  if ((!_config["wifi"].isNull()) && (_config["wifi"] != config["wifi"]))
  {
    netChange = 1;
    if (!_config["wifi"]["ssid"].isNull())
      config["wifi"]["ssid"] = _config["wifi"]["ssid"];
    if (!_config["wifi"]["pwd"].isNull())
      config["wifi"]["pwd"] = _config["wifi"]["pwd"];
  }
  if ((!_config["led"].isNull()) && (_config["led"] != config["led"]))
  {
    configChange = 1;
    if (!_config["led"]["power"].isNull())
      config["led"]["power"] = _config["led"]["power"];
    if (!_config["led"]["num"].isNull())
      config["led"]["num"] = _config["led"]["num"];
    if (!_config["led"]["brightness"].isNull())
      config["led"]["brightness"] = _config["led"]["brightness"];
    if (!_config["led"]["same"].isNull())
      config["led"]["same"] = _config["led"]["same"];
    if (!_config["led"]["color"].isNull())
      config["led"]["color"] = _config["led"]["color"];
  }
  if (netChange)
    setNetService();
  if (configChange)
    applyLEDConfig();
  if (netChange || configChange)
    saveConfig();
  FastLED.show();
}

void handleNotFound()
{
  webServer->send(404, "test/plain", "404: Not found");
}

void saveConfig()
{
  File config_file = SPIFFS.open("/config.json", "w");
  serializeJson(config, config_file);
  config_file.close();
}

void error()
{
  while (1)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
}