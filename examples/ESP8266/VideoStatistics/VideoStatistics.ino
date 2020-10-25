/**********************************************************
    This example show Youtube video stats, fetch through Youtube API.                                                  
    You can find extended library https://github.com/Ardutec/arduino-youtube-api
    based on orignal library https://github.com/witnessmenow/arduino-youtube-api
    
    Parts:
    Tested on NodeMCU ESP8266 12E
     
 ************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------
#include <YoutubeApi.h> 
// Library for connecting to the Youtube API
// This example works with this library
//https://github.com/Ardutec/arduino-youtube-api


#include <ArduinoJson.h>
// Library used for parsing Json from the API responses

// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson
// Library version 6.10.0

//------- Replace the following! ------
char ssid[] = "";       // your network SSID (name)
char password[] = "";  // your network key
#define API_KEY ""  // your google apps API Token
#define VIDEO_ID ""
//------- ---------------------- ------

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long timeBetweenRequests = 60000;
unsigned long nextRunTime;
long subs = 0;

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

// Required if you are using ESP8266 V2.5 or above
//  client.setInsecure();

// If you want to enable some extra debugging
  api._debug = true;
}

void loop() {

  if (millis() > nextRunTime)  {
    if(api.getVideoStatistics(VIDEO_ID))
    {
      Serial.println("---------Video Stats---------");
      Serial.print("Dislike Count: ");
      Serial.println(api.videoStats.dislikeCount);
      Serial.print("View Count: ");
      Serial.println(api.videoStats.viewCount);
      Serial.print("Comment Count: ");
      Serial.println(api.videoStats.commentCount);
      Serial.print("Like Count: ");
      Serial.println(api.videoStats.likeCount);
      Serial.println("------------------------");

    }
    nextRunTime = millis() + timeBetweenRequests;
  }
}