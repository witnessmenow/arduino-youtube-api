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
#include "YoutubePlaylist.h"
#include "YoutubeVideo.h"

// Library used for parsing Json from the API responses
// https://github.com/bblanchon/ArduinoJson
// (search for "Arduino Json" in the Arduino Library Manager)
#include <ArduinoJson.h>

//------- Replace the following! ------
const char ssid[] = "xxx";       // your network SSID (name)
const char password[] = "yyyy";  // your network key
#define API_KEY "zzzz"           // your Google API key
//-------------------------------------

#define playlistIdLen 24

#define timeBetweenRequestGroup 120 * 1000  // 120 seconds, in milliseconds 	| time between all requests
#define timeBetweenRequests 2 * 1000       // 2 seconds, in milliseconds    	| time between single requests

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

char playlistId[playlistIdLen + 1];
unsigned long startTime;

/**
 * @brief Tries to read the playlistId from Serial.
 * 
 * @return 1 on success, 0 if no data available
 */
int readPlaylist(){

	if(Serial.available() > playlistIdLen - 1){

			for(int i = 0; i < playlistIdLen; i++){

				playlistId[i] = Serial.read();
			}

			playlistId[playlistIdLen] = '\0';
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

void printTimestamp(tm t){
		Serial.print(t.tm_mday);
        Serial.print(".");
        Serial.print(t.tm_mon);
        Serial.print(".");
        Serial.print(t.tm_year + 1900);
        Serial.print(" ");
        Serial.print(t.tm_hour);
        Serial.print(":");
        Serial.print(t.tm_min);
        Serial.print(":");
        Serial.println(t.tm_sec);
}

void printPlaylistItemsPage(YoutubePlaylist *plist){

	playlistItemsConfiguration *config = plist->playlistItemsConfig;

	Serial.print("-------------------[Page ");
	Serial.print(config->currentPage + 1);
	Serial.print(" of ");
	Serial.print((int) ceil(((float) config->totalResults) / YT_PLAYLIST_ITEM_RESULTS_PER_PAGE));
	Serial.print(" | Entries: ");
	Serial.print(config->currentPageLastValidPos + 1);
	Serial.println(" ]-------------------");

	Serial.print("|----- previous Page token: ");
	if(strcmp("", config->previousPageToken)){
		Serial.println(config->previousPageToken);
	}else{
		Serial.println("[none]");
	}

	for(int entry = 0; entry < config->currentPageLastValidPos + 1; entry++){

		YoutubeVideo vid(plist->itemsContentDets[entry].videoId, plist->getYoutubeApiObj());
		vid.getVideoSnippet();

		Serial.print("\n|----- Entry: ");
		Serial.println(entry + 1);

		Serial.print("|----- video id: ");
		Serial.print(plist->itemsContentDets[entry].videoId);

		if(vid.checkVideoSnippetSet()){
			Serial.print("\t\t(\"");
			Serial.print(vid.videoSnip->title);
			Serial.println("\")");
		}else{
			Serial.print("\n");
		}

		Serial.print("|----- published at: ");
		printTimestamp(plist->itemsContentDets[entry].videoPublishedAt);
	}


	Serial.print("\n|----- next Page token: ");
	if(strcmp("", config->nextPageToken)){
		Serial.println(config->nextPageToken);
	}else{
		Serial.println("[none]");
	}

	Serial.println("-------------------------------------------------");
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
	Serial.print("Enter playlistId: ");

	while(1){
		if(readPlaylist()){
			flushSerialBuffer();
			break;
		}
	}

	Serial.println(playlistId);
}

void loop() {

	Serial.setTimeout(timeBetweenRequestGroup);

	YoutubePlaylist playlist(&api, playlistId);

	// fetch and print information in channel.list:snippet
	if(playlist.getPlaylistStatus()){
		Serial.println("\n\nstatus");

		playlistStatus *status = playlist.status;
        Serial.print("|----- privacy status: ");
        Serial.println(status->privacyStatus);

		Serial.println("-------------------------------------------------");
	}


	if(playlist.getPlaylistContentDetails()){
		Serial.println("\n\ncontentDetails");

		playlistContentDetails *contentDetails = playlist.contentDets;
        Serial.print("|----- item count: ");
        Serial.println(contentDetails->itemCount);

		Serial.println("-------------------------------------------------");
	}


	if(playlist.getPlaylistSnippet()){
		Serial.println("\n\nsnippet");


		playlistSnippet *snippet = playlist.snip;

		tm playlistCreation = snippet->publishedAt;

		Serial.print("|----- playlist creation (d.m.y h:m:s): ");
		Serial.print(playlistCreation.tm_mday);
        Serial.print(".");
        Serial.print(playlistCreation.tm_mon);
        Serial.print(".");
        Serial.print(playlistCreation.tm_year + 1900);
        Serial.print(" ");
        Serial.print(playlistCreation.tm_hour);
        Serial.print(":");
        Serial.print(playlistCreation.tm_min);
        Serial.print(":");
        Serial.println(playlistCreation.tm_sec);

        Serial.print("|----- channel id: ");
        Serial.println(snippet->channelId);

		Serial.print("|----- title: ");
        Serial.println(snippet->title);

		Serial.print("|----- description: ");
        Serial.println(snippet->description);

		Serial.print("|----- channel title: ");
        Serial.println(snippet->channelTitle);

		Serial.print("|----- default language: ");
        Serial.println(snippet->defaultLanguage);

		Serial.println("-------------------------------------------------");
	}

	Serial.println("\n");

	int page = 0;

	do{

		if(playlist.getPlaylistItemsPage(page)){
			printPlaylistItemsPage(&playlist);
		}

		page++;

	}while(page < (int) ceil(((float) playlist.playlistItemsConfig->totalResults) / YT_PLAYLIST_ITEM_RESULTS_PER_PAGE));

	Serial.print("\nRefreshing in ");
	Serial.print(timeBetweenRequestGroup / 1000.0);
	Serial.println(" seconds...");
	Serial.print("Or set a new playlistId: ");

	startTime = millis();
	flushSerialBuffer();

	while(millis() - startTime < timeBetweenRequestGroup){

		if(readPlaylist()){;
			Serial.println(playlistId);
			break;
		}
	}	
}
