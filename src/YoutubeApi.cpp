/*
   Copyright (c) 2017 Brian Lough. All right reserved.

   YoutubeApi - An Arduino wrapper for the YouTube API

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include "YoutubeApi.h"

YoutubeApi::YoutubeApi(String apiKey, Client &client)	{
	_apiKey = apiKey;
	this->client = &client;
}

String YoutubeApi::sendGetToYoutube(String command) {
	String headers="";
	String body="";
	bool finishedHeaders = false;
	bool currentLineIsBlank = true;
	unsigned long now;
	bool avail;
	// Connect with youtube api over ssl
	if (client->connect(YTAPI_HOST, YTAPI_SSL_PORT)) {
		if(_debug) { Serial.println(".... connected to server"); }
		String a="";
		char c;
		int ch_count=0;
		client->println("GET " + command + "&key=" + _apiKey + " HTTP/1.1");
		client->print("HOST: ");
		client->println(YTAPI_HOST);
		client->println();
		now=millis();
		avail=false;
		while (millis() - now < YTAPI_TIMEOUT) {
			while (client->available()) {

				// Allow body to be parsed before finishing
				avail = finishedHeaders;
				char c = client->read();
				//Serial.write(c);

				if(!finishedHeaders){
					if (currentLineIsBlank && c == '\n') {
						finishedHeaders = true;
					}
					else{
						headers = headers + c;

					}
				} else {
					if (ch_count < maxMessageLength)  {
						body=body+c;
						ch_count++;
					}
				}

				if (c == '\n') {
					currentLineIsBlank = true;
				}else if (c != '\r') {
					currentLineIsBlank = false;
				}
			}
			if (avail) {
				if(_debug) {
					Serial.println("Body:");
					Serial.println(body);
					Serial.println("END");
				}
				break;
			}
		}
	}
	closeClient();
	return body;
}

bool YoutubeApi::getChannelStatistics(String channelId){
	String command="/youtube/v3/channels?part=statistics&id="+channelId; //If you can't find it(for example if you have a custom url) look here: https://www.youtube.com/account_advanced
	if(_debug) { Serial.println(F("Closing client")); }
	String response = sendGetToYoutube(command);       //recieve reply from youtube
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
	if(root.success()) {
		if (root.containsKey("items")) {
			long subscriberCount = root["items"][0]["statistics"]["subscriberCount"];
			long viewCount = root["items"][0]["statistics"]["viewCount"];
			long commentCount = root["items"][0]["statistics"]["commentCount"];
			long hiddenSubscriberCount = root["items"][0]["statistics"]["hiddenSubscriberCount"];
			long videoCount = root["items"][0]["statistics"]["videoCount"];

			channelStats.viewCount = viewCount;
			channelStats.subscriberCount = subscriberCount;
			channelStats.commentCount = commentCount;
			channelStats.hiddenSubscriberCount = hiddenSubscriberCount;
			channelStats.videoCount = videoCount;

			return true;
		}
	}

	return false;
}

void YoutubeApi::closeClient() {
	if(client->connected()) {
		if(_debug) { Serial.println(F("Closing client")); }
		client->stop();
	}
}
