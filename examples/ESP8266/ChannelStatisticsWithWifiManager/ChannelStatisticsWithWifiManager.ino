/*******************************************************************
 *  Read YouTube Channel statistics from the YouTube API           *
 *  This sketch uses the WiFiManager Library for configuraiton     *
 *                                                                 *
 *  By Brian Lough                                                 *
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA       *
 *******************************************************************/

#include <YoutubeApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// For storing configurations
#include "FS.h"
#include <ArduinoJson.h>

// WiFiManager Libraries
#include <DNSServer.h>            //Local DNS Server used for redirecting all rs to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

const int resetConfigPin = D8; //When high will reset the wifi manager config

char apiKey[45] = "";
char channelId[30] = "UCezJOfu7OtqGzd5xrP3q6WA";

WiFiClientSecure client;
YoutubeApi *api;

unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_lasttime;   //last time api request has been done

long subs = 0;

// flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {

  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount FS");
    return;
  }

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  loadConfig();

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // Adding an additional config on the WIFI manager webpage for the API Key and Channel ID
  WiFiManagerParameter customApiKey("apiKey", "API Key", apiKey, 50);
  WiFiManagerParameter customChannelId("channelId", "Channel ID", channelId, 35);
  wifiManager.addParameter(&customApiKey);
  wifiManager.addParameter(&customChannelId);

  // If it fails to connect it will create a YouTube-Counter access point
  wifiManager.autoConnect("YouTube-Counter", "supersecret");

  strcpy(apiKey, customApiKey.getValue());
  strcpy(channelId, customChannelId.getValue());

  if (shouldSaveConfig) {
    saveConfig();
  }

  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  // Force Config mode if there is no API key
  if(strcmp(apiKey, "") > 0) {
    Serial.println("Init YouTube API");
    api = new YoutubeApi(apiKey, client);
  } else {
    Serial.println("Forcing Config Mode");
    forceConfigMode();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  strcpy(apiKey, json["apiKey"]);
  strcpy(channelId, json["channelId"]);
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["apiKey"] = apiKey;
  json["channelId"] = channelId;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

void forceConfigMode() {
  Serial.println("Reset");
  WiFi.disconnect();
  Serial.println("Dq");
  delay(500);
  ESP.restart();
  delay(5000);
}

void loop() {

  if (digitalRead(resetConfigPin) == HIGH) {
    forceConfigMode();
  }
  if (millis() - api_lasttime > api_mtbs) {
    if(api->getChannelStatistics(channelId))
    {
      Serial.println("---------Stats---------");
      Serial.print("Subscriber Count: ");
      Serial.println(api->channelStats.subscriberCount);
      Serial.print("View Count: ");
      Serial.println(api->channelStats.viewCount);
      Serial.print("Comment Count: ");
      Serial.println(api->channelStats.commentCount);
      Serial.print("Video Count: ");
      Serial.println(api->channelStats.videoCount);
      // Probably not needed :)
      //Serial.print("hiddenSubscriberCount: ");
      //Serial.println(api->channelStats.hiddenSubscriberCount);
      Serial.println("------------------------");

    }
    api_lasttime = millis();
  }
}
