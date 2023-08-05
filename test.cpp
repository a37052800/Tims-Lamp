#include <iostream>
#include "ArduinoJson-v6.21.3.h"
using namespace std;
int main()
{
    DynamicJsonDocument config(256);
    config["mode"] = "ap";
    config["wifi"]["ssid"] = "Lamp_Contorl";
    config["wifi"]["pwd"] = "password";
    config["led"]["power"] = "off";
    config["led"]["num"] = 30;
    config["led"]["brightness"] = 128;
    config["led"]["same"] = "true";
    config["led"]["color"] = "0xFFFFFF";

    cout<<config["wifi"];
    // deserializeJson(config,"{\"mode\":\"dd\"}");
    // cout<<config["led"]["power"]<<"||"<<config["mode"]<<'\n';
    // if(config["led"]["power"].isNull()) cout<<"YES";
    // if(config["led"]["power"]==nullptr) cout<<"YES";
}