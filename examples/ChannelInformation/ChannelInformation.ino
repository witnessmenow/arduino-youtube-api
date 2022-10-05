/*******************************************************************
	Read YouTube Channel information from the YouTube API
	and print them to the serial monitor

	Compatible Boards:
	* Any ESP8266 board
	* Any ESP32 board

	Recommended Board: D1 Mini ESP8266
	http://s.click.aliexpress.com/e/uzFUnIe (affiliate)

	If you find what I do useful and would like to support me,
	please consider becoming a sponsor on Github
	https://github.com/sponsors/witnessmenow/

	Written by Brian Lough
	YouTube: https://www.youtube.com/brianlough
	Tindie: https://www.tindie.com/stores/brianlough/
	Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#if defined(ESP8266)
	#include <ESP8266WiFi.h>
#elif defined(ESP32)
	#include <WiFi.h>
#endif

#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each of these will need to be installed
// ----------------------------

// Library for connecting to the YouTube API
// https://github.com/witnessmenow/arduino-youtube-api
// (search for "youtube" in the Arduino Library Manager)
#include "YoutubeApi.h"
#include "YoutubeChannel.h"

// Library used for parsing Json from the API responses
// https://github.com/bblanchon/ArduinoJson
// (search for "Arduino Json" in the Arduino Library Manager)
#include <ArduinoJson.h>

//------- Replace the following! ------
const char ssid[] = "xxx";       // your network SSID (name)
const char password[] = "yyyy";  // your network key
#define API_KEY "zzzz"           // your Google API key
#define CHANNEL_ID "UCezJOfu7OtqGzd5xrP3q6WA" // part of the channel url
//------- ---------------------- ------

#define channelIdLen 24

#define timeBetweenRequestGroup 120 * 1000  // 120 seconds, in milliseconds 	| time between all requests
#define timeBetweenRequests 2 * 1000       // 2 seconds, in milliseconds    	| time between single requests

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

char videoId[channelIdLen + 1];
unsigned long startTime;

/**
 * @brief Tries to read the videoId from Serial.
 * 
 * @return 1 on success, 0 if no data available
 */
int readVideoId(){

	if(Serial.available() > channelIdLen - 1){

			for(int i = 0; i < channelIdLen; i++){

				videoId[i] = Serial.read();
			}

			videoId[channelIdLen] = '\0';
			return 1;
	}
	return 0;
}

/**
 * @brief Flushes the Serial input buffer.
 * 
 */
void flushSerialBuffer(){
	while(Serial.available()){
		Serial.read();
	}
}

/**
 * @brief Prints "Yes\n" if x or "No\n" if not x 
 * 
 * @param x parameter
 */
void printYesNo(bool x){
	if(x){
		Serial.println("Yes");
	}else{
		Serial.println("No");
	}
}


void setup() {
	Serial.begin(115200);
	
	// Set WiFi to 'station' mode and disconnect
	// from the AP if it was previously connected
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	// Connect to the WiFi network
	Serial.print("\nConnecting to WiFi: ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}
	Serial.println("\nWiFi connected!");
	Serial.print("IP address: ");
	IPAddress ip = WiFi.localIP();
	Serial.println(ip);

	#ifdef ESP8266
	// Required if you are using ESP8266 V2.5 or above
	client.setInsecure();
	#endif
		
	client.setInsecure();
	
	// Uncomment for extra debugging info
	// api._debug = true;

	flushSerialBuffer();
	Serial.print("Enter channelId: ");

	while(1){
		if(readVideoId()){
			flushSerialBuffer();
			break;
		}
	}

	Serial.println(videoId);
}

void loop() {

	Serial.setTimeout(timeBetweenRequestGroup);

	YoutubeChannel channel(videoId, &api);

	// fetch and print information in channel.list:snippet
	if(channel.getChannelSnippet()){
		Serial.println("\n\nsnippet");

		channelSnippet *fetchedSnip = channel.channelSnip;


		Serial.print("|----- Channel title: ");
		Serial.println(fetchedSnip->title);

		Serial.print("|----- Channel description: ");
		Serial.println(fetchedSnip->description);

		Serial.print("|----- Channel country: ");
		Serial.println(fetchedSnip->country);

		tm channelCreation = fetchedSnip->publishedAt;

		Serial.print("|----- Channel creation (d.m.y h:m:s): ");
		Serial.print(channelCreation.tm_mday);
        Serial.print(".");
        Serial.print(channelCreation.tm_mon);
        Serial.print(".");
        Serial.print(channelCreation.tm_year + 1900);
        Serial.print(" ");
        Serial.print(channelCreation.tm_hour);
        Serial.print(":");
        Serial.print(channelCreation.tm_min);
        Serial.print(":");
        Serial.println(channelCreation.tm_sec);

		Serial.println("-------------------------------------------------");
	}

	if(channel.getChannelStatistics()){
		Serial.println("\n\nstatistics");

		channelStatistics *fetchedStats = channel.channelStats;

		Serial.print("|----- Channel views: ");
		Serial.println(fetchedStats->viewCount);

		Serial.print("|----- Channel subscriber count hidden? ");
		printYesNo(fetchedStats->hiddenSubscriberCount);

		Serial.print("|----- Channel subscribers: ");
		Serial.println(fetchedStats->subscriberCount);

		Serial.print("|----- Channel video count: ");
		Serial.println(fetchedStats->videoCount);

		Serial.println("-------------------------------------------------");
	}

	delay(timeBetweenRequests);

	// fetch and print information in channel.list:contentDetails
	if(channel.getChannelContentDetails()){
		Serial.println("\n\ncontent details");

		channelContentDetails *fetchedDetails = channel.channelContentDets;

		Serial.print("|----- Liked videos playlist id: ");
		Serial.println(fetchedDetails->relatedPlaylistsLikes);

		Serial.print("|----- Uploaded videos playlist id: ");
		Serial.println(fetchedDetails->relatedPlaylistsUploads);

		Serial.println("-------------------------------------------------");
	}

	Serial.print("\nRefreshing in ");
	Serial.print(timeBetweenRequestGroup / 1000.0);
	Serial.println(" seconds...");
	Serial.print("Or set a new channelId: ");

	startTime = millis();
	flushSerialBuffer();

	while(millis() - startTime < timeBetweenRequestGroup){

		if(readVideoId()){;
			Serial.println(videoId);
			break;
		}
	}	
}
