/*******************************************************************
	Read all available YouTube Video information from the YouTube API
	and print them to the serial monitor

	Compatible Boards:
	* Any ESP8266 board
	* Any ESP32 board

	Recommended Board: D1 Mini ESP8266
	http://s.click.aliexpress.com/e/uzFUnIe (affiliate)

	If you find what I do useful and would like to support me,
	please consider becoming a sponsor on Github
	https://github.com/sponsors/witnessmenow/

	Written by Brian Lough and modified by Colum31
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
#include "YoutubeVideo.h"

// Library used for parsing Json from the API responses
// https://github.com/bblanchon/ArduinoJson
// (search for "Arduino Json" in the Arduino Library Manager)
#include <ArduinoJson.h>

//------- Replace the following! ------
#define WIFI_SSID  "xxxx"  // your network SSID (name)
#define WIFI_PASSWORD  "yyyy"   // your network key
#define API_KEY "zzzz" // your Google API key
//------- ---------------------- ------



#define timeBetweenRequestGroup 120 * 1000  // 120 seconds, in milliseconds 	| time between all requests
#define timeBetweenRequests 2 * 1000       // 2 seconds, in milliseconds    	| time between single requests
#define videoIdLen 11

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

char videoId[videoIdLen + 1];
unsigned long startTime;

/**
 * @brief Tries to read the videoId from Serial.
 * 
 * @return 1 on success, 0 if no data available
 */
int readVideoId(){

	if(Serial.available() > videoIdLen - 1){

			for(int i = 0; i < videoIdLen; i++){

				videoId[i] = Serial.read();
			}

			videoId[videoIdLen] = '\0';
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
	Serial.println(WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
	Serial.print("Enter videoId: ");

	while(1){
		if(readVideoId()){
			flushSerialBuffer();
			break;
		}
	}

	Serial.println(videoId);
}

void loop() {

	YoutubeVideo vid(videoId, &api);
	Serial.setTimeout(timeBetweenRequestGroup);


	// fetch and print information in videos.list:statistics
	if(vid.getVideoStatistics()){
		Serial.println("\n\nstatistics");

		Serial.print("|----- Video views: ");
		Serial.println(vid.videoStats->viewCount);
		
		Serial.print("|----- Likes: ");
		Serial.println(vid.videoStats->likeCount);

		Serial.print("|----- Comments: ");
		Serial.println(vid.videoStats->commentCount);

		Serial.println("-------------------------------------------------");
	}

	

	// fetch and print information in videos.list:snippet
	if(vid.getVideoSnippet()){
		Serial.println("\n\nsnippet");

		Serial.print("|----- Video title: ");
		Serial.println(vid.videoSnip->title);
		
		Serial.println("|----- Video description: \n");
		Serial.println(vid.videoSnip->description);
        Serial.println("");

		Serial.print("|----- Uploaded by (channel title): ");
		Serial.println(vid.videoSnip->channelTitle);

        Serial.print("|----- Uploaded by (channel id): ");
		Serial.println(vid.videoSnip->channelId);

        Serial.print("|----- Published at (d.m.y h:m:s): ");
		Serial.print(vid.videoSnip->publishedAt.tm_mday);
        Serial.print(".");
        Serial.print(vid.videoSnip->publishedAt.tm_mon);
        Serial.print(".");
        Serial.print(vid.videoSnip->publishedAt.tm_year + 1900);
        Serial.print(" ");
        Serial.print(vid.videoSnip->publishedAt.tm_hour);
        Serial.print(":");
        Serial.print(vid.videoSnip->publishedAt.tm_min);
        Serial.print(":");
        Serial.println(vid.videoSnip->publishedAt.tm_sec);

        Serial.print("|----- Livebroadcast content: ");
        Serial.println(vid.videoSnip->liveBroadcastContent);

        Serial.print("|----- Category id: ");
        Serial.println(vid.videoSnip->categoryId);

        Serial.print("|----- Default language: ");
        Serial.println(vid.videoSnip->defaultLanguage);


        Serial.print("|----- Default audio language: ");
        Serial.println(vid.videoSnip->defaultAudioLanguage);
        
		Serial.println("-------------------------------------------------");
	}

    delay(timeBetweenRequests);

	// fetch and print information in videos.list:contentDetails
    if(vid.getVideoContentDetails()){
		Serial.println("\n\ncontentDetails");

		Serial.print("|----- Video duration ");

        if(vid.videoContentDets->duration.tm_mday != 0){
            Serial.print("(d:h:m:s): ");
            Serial.print(vid.videoContentDets->duration.tm_mday);
            Serial.print(":");
        }else{
            Serial.print("(h:m:s): ");
        }

        
        Serial.print(vid.videoContentDets->duration.tm_hour);
        Serial.print(":");
        Serial.print(vid.videoContentDets->duration.tm_min);
        Serial.print(":");
        Serial.println(vid.videoContentDets->duration.tm_sec);

		Serial.print("|----- Dimension: ");
		Serial.println(vid.videoContentDets->dimension);

		Serial.print("|----- Definition: ");
		Serial.println(vid.videoContentDets->defintion);

		Serial.print("|----- Captioned: ");
		printYesNo(vid.videoContentDets->caption);

		Serial.print("|----- Licensed Content: ");
		printYesNo(vid.videoContentDets->licensedContent);

		Serial.print("|----- Projection: ");
		Serial.println(vid.videoContentDets->projection);

		Serial.println("-------------------------------------------------");
	}
	
	delay(timeBetweenRequests);

	if(vid.getVideoStatus()){
		Serial.println("\n\n status");

		Serial.print("|----- upload status: ");
		Serial.println(vid.vStatus->uploadStatus);

		Serial.print("|----- privacy status: ");
		Serial.println(vid.vStatus->privacyStatus);

		Serial.print("|----- license: ");
		Serial.println(vid.vStatus->license);

		Serial.print("|----- embeddable: ");
		printYesNo(vid.vStatus->embeddable);

		Serial.print("|----- public stats viewable: ");
		printYesNo(vid.vStatus->publicStatsViewable);

		Serial.print("|----- made for kids: ");
		printYesNo(vid.vStatus->madeForKids);
	}

	
	
	Serial.print("\nRefreshing in ");
	Serial.print(timeBetweenRequestGroup / 1000.0);
	Serial.println(" seconds...");
	Serial.print("Or set a new videoId: ");

	startTime = millis();
	flushSerialBuffer();

	while(millis() - startTime < timeBetweenRequestGroup){

		if(readVideoId()){;
			Serial.println(videoId);
			break;
		}
	}	
}
