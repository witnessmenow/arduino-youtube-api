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

#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

//------- Replace the following! ------
char channelId[30] = "";
char clientId[80] = "";
char clientSecret[120] = "";
char refreshToken[80] = "";

WiFiClientSecure client;
YoutubeApi *api;

unsigned long api_mtbs = 5000; //mean time between api requests
unsigned long api_lasttime;   //last time api request has been done

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void loop() {
  if (millis() - api_lasttime > api_mtbs) {
    String result = api->getMyRecentSubscribers(); // Returns the nextPageToken or the error message
    Serial.println("--My Recent Subscribers--");
    for (int i = 0; i < sizeof(api->myRecentSubscribers)/sizeof(String); i++) {
      Serial.println(api->myRecentSubscribers[i]);
    }
    Serial.print("Result (nextPageToken or error): ");
    Serial.println(result);
    Serial.println("------------------------");

    api_lasttime = millis();
  }
}
