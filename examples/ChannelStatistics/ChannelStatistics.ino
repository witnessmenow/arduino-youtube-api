/*******************************************************************
	Read YouTube Channel statistics from the YouTube API
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
#include <YoutubeApi.h>

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

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long timeBetweenRequests = 60 * 1000;  // 60 seconds, in milliseconds

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
}

void loop() {
	if(api.getChannelStatistics(CHANNEL_ID)) {
		Serial.println("\n---------Stats---------");

		Serial.print("Subscriber Count: ");
		Serial.println(api.channelStats.subscriberCount);

		Serial.print("View Count: ");
		Serial.println(api.channelStats.viewCount);

		Serial.print("Video Count: ");
		Serial.println(api.channelStats.videoCount);

		// Probably not needed :)
		//Serial.print("hiddenSubscriberCount: ");
		//Serial.println(api.channelStats.hiddenSubscriberCount);

		Serial.println("------------------------");
	}
	delay(timeBetweenRequests);
}
