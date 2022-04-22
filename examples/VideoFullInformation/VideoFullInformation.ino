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
#include <YoutubeApi.h>

// Library used for parsing Json from the API responses
// https://github.com/bblanchon/ArduinoJson
// (search for "Arduino Json" in the Arduino Library Manager)
#include <ArduinoJson.h>

//------- Replace the following! ------
const char ssid[] = "xxxx";  // your network SSID (name)
const char password[] = "yyyy";  // your network key
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

	Serial.setTimeout(timeBetweenRequestGroup);

	// fetch and print information in videos.list:snippet
	if(api.getVideoSnippet(videoId)){
		Serial.println("\n\nsnippet");

		Serial.print("|----- Video title: ");
		Serial.println(api.videoSnip.title);
		
		Serial.println("|----- Video description: \n");
		Serial.println(api.videoSnip.description);
        Serial.println("");

		Serial.print("|----- Uploaded by (channel title): ");
		Serial.println(api.videoSnip.channelTitle);

        Serial.print("|----- Uploaded by (channel id): ");
		Serial.println(api.videoSnip.channelId);

        Serial.print("|----- Published at (d.m.y h:m:s): ");
		Serial.print(api.videoSnip.publishedAt.tm_mday);
        Serial.print(".");
        Serial.print(api.videoSnip.publishedAt.tm_mon);
        Serial.print(".");
        Serial.print(api.videoSnip.publishedAt.tm_year + 1900);
        Serial.print(" ");
        Serial.print(api.videoSnip.publishedAt.tm_hour);
        Serial.print(":");
        Serial.print(api.videoSnip.publishedAt.tm_min);
        Serial.print(":");
        Serial.println(api.videoSnip.publishedAt.tm_sec);

        Serial.print("|----- Livebroadcast content: ");
        Serial.println(api.videoSnip.liveBroadcastContent);

        Serial.print("|----- Category id: ");
        Serial.println(api.videoSnip.categoryId);

        Serial.print("|----- Default language: ");
        Serial.println(api.videoSnip.defaultLanguage);


        Serial.print("|----- Default audio language: ");
        Serial.println(api.videoSnip.defaultAudioLanguage);
        
		Serial.println("-------------------------------------------------");
	}

    delay(timeBetweenRequests);

	// fetch and print information in videos.list:statistics
	if(api.getVideoStatistics(videoId)){
		Serial.println("\n\nstatistics");

		Serial.print("|----- Video views: ");
		Serial.println(api.videoStats.viewCount);
		
		Serial.print("|----- Likes: ");
		Serial.println(api.videoStats.likeCount);

		Serial.print("|----- Comments: ");
		Serial.println(api.videoStats.commentCount);

		Serial.println("-------------------------------------------------");
	}

    delay(timeBetweenRequests);

	// fetch and print information in videos.list:contentDetails
    if(api.getVideoContentDetails(videoId)){
		Serial.println("\n\ncontentDetails");

		Serial.print("|----- Video duration ");

        if(api.videoContentDets.duration.tm_mday != 0){
            Serial.print("(d:h:m:s): ");
            Serial.print(api.videoContentDets.duration.tm_mday);
            Serial.print(":");
        }else{
            Serial.print("(h:m:s): ");
        }

        
        Serial.print(api.videoContentDets.duration.tm_hour);
        Serial.print(":");
        Serial.print(api.videoContentDets.duration.tm_min);
        Serial.print(":");
        Serial.println(api.videoContentDets.duration.tm_sec);

		Serial.print("|----- Dimension: ");
		Serial.println(api.videoContentDets.dimension);

		Serial.print("|----- Definition: ");
		Serial.println(api.videoContentDets.defintion);

		Serial.print("|----- Captioned: ");
		printYesNo(api.videoContentDets.caption);

		Serial.print("|----- Licensed Content: ");
		printYesNo(api.videoContentDets.licensedContent);

		Serial.print("|----- Projection: ");
		Serial.println(api.videoContentDets.projection);

		Serial.println("-------------------------------------------------");
	}

	delay(timeBetweenRequests);

	if(api.getVideoStatus(videoId)){
		Serial.println("\n\n status");

		Serial.print("|----- upload status: ");
		Serial.println(api.vStatus.uploadStatus);

		Serial.print("|----- privacy status: ");
		Serial.println(api.vStatus.privacyStatus);

		Serial.print("|----- license: ");
		Serial.println(api.vStatus.license);

		Serial.print("|----- embeddable: ");
		printYesNo(api.vStatus.embeddable);

		Serial.print("|----- public stats viewable: ");
		printYesNo(api.vStatus.publicStatsViewable);

		Serial.print("|----- made for kids: ");
		printYesNo(api.vStatus.madeForKids);
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
