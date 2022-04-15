/*
 * YoutubeApi - An Arduino wrapper for the YouTube API
 * Copyright (c) 2020 Brian Lough
 * 
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */


#include "YoutubeApi.h"

YoutubeApi::YoutubeApi(const char* key, Client &client)
	: apiKey(key), client(client)
{}

YoutubeApi::YoutubeApi(const String &apiKey, Client &client) 
	: YoutubeApi(apiKey.c_str(), client)  // passing the key as c-string to force a copy
{}

int YoutubeApi::sendGetToYoutube(const char *command) {
	client.flush();
	client.setTimeout(YTAPI_TIMEOUT);
	if (!client.connect(YTAPI_HOST, YTAPI_SSL_PORT))
	{
		Serial.println(F("Connection failed"));
		return false;
	}
	// give the esp a breather
	yield();

	// Send HTTP request
	client.print(F("GET "));
	client.print(command);
	client.println(F(" HTTP/1.1"));

	//Headers
	client.print(F("Host: "));
	client.println(YTAPI_HOST);

	client.println(F("Cache-Control: no-cache"));

	if (client.println() == 0)
	{
		Serial.println(F("Failed to send request"));
		return -1;
	}

	int statusCode = getHttpStatusCode();

	// Let the caller of this method parse the JSon from the client
	skipHeaders();
	return statusCode;
}

int YoutubeApi::sendGetToYoutube(const String& command) {
	return sendGetToYoutube(command.c_str());
}

bool YoutubeApi::getChannelStatistics(const char *channelId) {
	char command[150] = YTAPI_CHANNEL_ENDPOINT;
	char params[120];
	sprintf(params, "?part=statistics&id=%s&key=%s", channelId, apiKey.c_str());
	strcat(command, params);

	if (_debug)
	{
		Serial.println(command);
	}

	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = JSON_ARRAY_SIZE(1) 
	                        + JSON_OBJECT_SIZE(2) 
	                        + 2*JSON_OBJECT_SIZE(4) 
	                        + JSON_OBJECT_SIZE(5)
	                        + 330;

	int httpStatus = sendGetToYoutube(command);

	if (httpStatus == 200)
	{
		// Allocate DynamicJsonDocument
		DynamicJsonDocument doc(bufferSize);

		// Parse JSON object
		DeserializationError error = deserializeJson(doc, client);
		if (!error)
		{
			wasSuccessful = true;

			JsonObject itemStatistics = doc["items"][0]["statistics"];

			channelStats.viewCount = itemStatistics["viewCount"].as<long>();
			channelStats.subscriberCount = itemStatistics["subscriberCount"].as<long>();
			channelStats.commentCount = itemStatistics["commentCount"].as<long>();
			channelStats.hiddenSubscriberCount = itemStatistics["hiddenSubscriberCount"].as<bool>();
			channelStats.videoCount = itemStatistics["videoCount"].as<long>();
		}
		else
		{
			Serial.print(F("deserializeJson() failed with code "));
			Serial.println(error.c_str());
		}
	} else {
		Serial.print("Unexpected HTTP Status Code: ");
		Serial.println(httpStatus);
	}
	closeClient();

	return wasSuccessful;
}

bool YoutubeApi::getChannelStatistics(const String& channelId) {
	return getChannelStatistics(channelId.c_str());
}


/**
 * @brief Gets the statistics of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return wasSuccesssful true, if there were no errors and the video was found
 */
bool YoutubeApi::getVideoStatistics(const char *videoId){
	char command[150] = YTAPI_VIDEO_ENDPOINT;
	char params[120];
	sprintf(params, "?part=statistics&id=%s&key=%s", videoId, apiKey.c_str());
	strcat(command, params);

	if (_debug)
	{
		Serial.println(command);
	}

	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = JSON_ARRAY_SIZE(1) 
	                        + JSON_OBJECT_SIZE(2) 
	                        + 2*JSON_OBJECT_SIZE(4) 
	                        + JSON_OBJECT_SIZE(5)
	                        + 330;

	int httpStatus = sendGetToYoutube(command);

	if (httpStatus == 200)
	{
		// Allocate DynamicJsonDocument
		DynamicJsonDocument doc(bufferSize);

		// Parse JSON object
		DeserializationError error = deserializeJson(doc, client);

		if (error){
			Serial.print(F("deserializeJson() failed with code "));
			Serial.println(error.c_str());
		}
		else if(doc["pageInfo"]["totalResults"].as<int>() == 0){
			Serial.print("No results found for video id ");
			Serial.println(videoId);
		}
		else{
			wasSuccessful = true;

			JsonObject itemStatistics = doc["items"][0]["statistics"];

			videoStats.viewCount = itemStatistics["viewCount"].as<long>();
			videoStats.likeCount = itemStatistics["likeCount"].as<long>();
			videoStats.commentCount= itemStatistics["commentCount"].as<long>();
		}
	} else {
		Serial.print("Unexpected HTTP Status Code: ");
		Serial.println(httpStatus);
	}
	closeClient();

	return wasSuccessful;
}

bool YoutubeApi::getVideoStatistics(const String& videoId){
	return getVideoStatistics(videoId.c_str());
}

/**
 * @brief Parses the ISO8601 duration string into a tm time struct.
 * 
 * @param duration Pointer to string
 * @return tm Time struct corresponding to duration. When sucessful, it's non zero.
 */
tm YoutubeApi::parseDuration(const char *duration){

	tm ret;
	memset(&ret, 0, sizeof(tm));

	if(!duration){
		return ret;
	}

	char temp[3];

	int len = strlen(duration);
	int marker = len - 1;

	bool secondsSet, minutesSet, hoursSet, daysSet;
	secondsSet = minutesSet = hoursSet = daysSet = false;

	for(int i = len - 1; i >= 0; i--){
		
		char c = duration[i];

		if(c == 'S'){
			secondsSet = true;
			marker = i - 1;
			continue;
		}

		if(c == 'M'){
			minutesSet = true;

			if(secondsSet){
				memcpy(&temp, &duration[i + 1], marker - i + 1);
				ret.tm_sec = atoi(temp);

				secondsSet = false;
			}
			marker = i - 1;
			continue;
		}

		if(c == 'H'){
			hoursSet = true;

			if(secondsSet || minutesSet){

				memcpy(&temp, &duration[i + 1], marker - i + 1);
				int time = atoi(temp);

				if(secondsSet){
					ret.tm_sec = time;
					secondsSet = false;
				}else{
					ret.tm_min = time;
					minutesSet = false;
				}
			}

			marker = i - 1;
			continue;
		}

		if(c == 'T'){

			if(secondsSet || minutesSet || hoursSet){

				memcpy(&temp, &duration[i + 1], marker - i + 1);
				int time = atoi(temp);

				if(secondsSet){
					ret.tm_sec = time;
				}else if (minutesSet){
					ret.tm_min = time;
				}else{
					ret.tm_hour = time;
				}
			}
		}
		// a video can be as long as days
		if(c == 'D'){
			marker = i - 1;
			daysSet = true;
		}

		if(c == 'P' && daysSet){

			memcpy(&temp, &duration[i + 1], marker - i + 1);
			int time = atoi(temp);

			ret.tm_mday = time;
		
		}
	}
	return ret;
}


/**
 * @brief Gets the content details of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return wasSuccesssful true, if there were no errors and the video was found
 */
bool YoutubeApi::getContentDetails(const char *videoId){
	char command[150] = YTAPI_VIDEO_ENDPOINT;
	char params[120];
	sprintf(params, "?part=contentDetails&id=%s&key=%s", videoId, apiKey.c_str());
	strcat(command, params);

	if (_debug)
	{
		Serial.println(command);
	}

	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 768;

	int httpStatus = sendGetToYoutube(command);

	if (httpStatus == 200)
	{	
		// Creating a filter to filter out 
		// region restrictions, content rating and metadata
		DynamicJsonDocument filter(144);

		JsonObject filterItems = filter["items"][0].createNestedObject("contentDetails");
		filterItems["duration"] = true;
		filterItems["dimension"] = true;
		filterItems["definition"] = true;
		filterItems["caption"] = true;
		filterItems["licensedContent"] = true;
		filter["pageInfo"] = true;

		// Allocate DynamicJsonDocument
		DynamicJsonDocument doc(bufferSize);

		// Parse JSON object
		DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter));

		// check for errors and empty response
		if(error){
			Serial.print(F("deserializeJson() failed with code "));
			Serial.println(error.c_str());
		}
		else if(doc["pageInfo"]["totalResults"].as<int>() == 0){
			Serial.print("No results found for video id ");
			Serial.println(videoId);
		}
		else{
			wasSuccessful = true;

			JsonObject itemcontentDetails = doc["items"][0]["contentDetails"];

			memcpy(contentDets.defintion, itemcontentDetails["definition"].as<const char *>(), 3);
			
			memcpy(contentDets.dimension, itemcontentDetails["dimension"].as<const char *>(), 3);
			
			if(strcmp("false", itemcontentDetails["caption"].as<const char*>()) != 0){
				contentDets.caption = true;
			}
			else{
				contentDets.caption = false;
			}
			
			contentDets.licensedContent = itemcontentDetails["licensedContent"].as<bool>();
			contentDets.duration = parseDuration(itemcontentDetails["duration"].as<const char*>());
		}
		
	} else {
		Serial.print("Unexpected HTTP Status Code: ");
		Serial.println(httpStatus);
	}
	closeClient();

	return wasSuccessful;
}
bool YoutubeApi::getContentDetails(const String& videoId){
	return getContentDetails(videoId.c_str());
}

void YoutubeApi::skipHeaders() {
	// Skip HTTP headers
	char endOfHeaders[] = "\r\n\r\n";
	if (!client.find(endOfHeaders))
	{
		Serial.println(F("Invalid response"));
		return;
	}

	// Was getting stray characters between the headers and the body
	// This should toss them away
	while (client.available() && client.peek() != '{')
	{
		char c = 0;
		client.readBytes(&c, 1);
		if (_debug)
		{
			Serial.print("Tossing an unexpected character: ");
			Serial.println(c);
		}
	}
}

int YoutubeApi::getHttpStatusCode() {
	// Check HTTP status
	if(client.find("HTTP/1.1")){
		int statusCode = client.parseInt();
		return statusCode;
	} 

	return -1;
}

void YoutubeApi::closeClient() {
	if(client.connected()) {
		if(_debug) { Serial.println(F("Closing client")); }
		client.stop();
	}
}
