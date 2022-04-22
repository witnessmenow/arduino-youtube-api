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

// TODO
//
// add 	video.list:topicDetails

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


/**
 * @brief Parses the channel statistics from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeApi::parseChannelStatistics() {
	
	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = JSON_ARRAY_SIZE(1) 
	                        + JSON_OBJECT_SIZE(2) 
	                        + 2*JSON_OBJECT_SIZE(4) 
	                        + JSON_OBJECT_SIZE(5)
	                        + 330;
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, client);
	if (!error){
		JsonObject itemStatistics = doc["items"][0]["statistics"];

		channelStats.viewCount = itemStatistics["viewCount"].as<long>();
		channelStats.subscriberCount = itemStatistics["subscriberCount"].as<long>();
		channelStats.commentCount = itemStatistics["commentCount"].as<long>();
		channelStats.hiddenSubscriberCount = itemStatistics["hiddenSubscriberCount"].as<bool>();
		channelStats.videoCount = itemStatistics["videoCount"].as<long>();

		wasSuccessful = true;
	}
	else{
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}

	closeClient();
	return wasSuccessful;
}


/**
 * @brief Parses the video statistics from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeApi::parseVideoStatistics(){
	
	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = JSON_ARRAY_SIZE(1) 
	                        + JSON_OBJECT_SIZE(2) 
	                        + 2*JSON_OBJECT_SIZE(4) 
	                        + JSON_OBJECT_SIZE(5)
	                        + 330;

	// Allocate DynamicJsonDocument
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, client);

	if (error){
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}
	else if(doc["pageInfo"]["totalResults"].as<int>() == 0){
		Serial.println("No results found for video id ");
	}
	else{
		JsonObject itemStatistics = doc["items"][0]["statistics"];

		videoStats.viewCount = itemStatistics["viewCount"].as<long>();
		videoStats.likeCount = itemStatistics["likeCount"].as<long>();
		videoStats.commentCount= itemStatistics["commentCount"].as<long>();

		wasSuccessful = true;
	}

	closeClient();
	return wasSuccessful;
}


/**
 * @brief Parses the video content details from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeApi::parseContentDetails(){
	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 384;

	
	// Creating a filter to filter out 
	// region restrictions, content rating and metadata
	StaticJsonDocument<180> filter;

	JsonObject filterItems = filter["items"][0].createNestedObject("contentDetails");
	filterItems["duration"] = true;
	filterItems["dimension"] = true;
	filterItems["definition"] = true;
	filterItems["caption"] = true;
	filterItems["licensedContent"] = true;
	filterItems["projection"] = true;
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
		Serial.println("No results found for video id ");
	}
	else{
		wasSuccessful = true;

		JsonObject itemcontentDetails = doc["items"][0]["contentDetails"];

		memcpy(contentDets.defintion, itemcontentDetails["definition"].as<const char *>(), 3);
		memcpy(contentDets.dimension, itemcontentDetails["dimension"].as<const char *>(), 3);
		strcpy(contentDets.projection, itemcontentDetails["projection"].as<const char*>());

		if("false" == itemcontentDetails["caption"]){
			contentDets.caption = true;
		}
		else{
			contentDets.caption = false;
		}
			
		contentDets.licensedContent = itemcontentDetails["licensedContent"].as<bool>();
		contentDets.duration = parseDuration(itemcontentDetails["duration"].as<const char*>());
	}
		
	closeClient();
	return wasSuccessful;
}


/**
 * @brief Parses the video snippet from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeApi::parseSnippet(){

	bool wasSuccessful = false;

	// should be more, just to test
	// description can be as large as 5kb, title 400 bytes
	const size_t bufferSize = 6000;

	// Creating a filter to filter out 
	// metadata, thumbnail links, tags, localized information
	StaticJsonDocument<256> filter;

	JsonObject filterItems = filter["items"][0].createNestedObject("snippet");
	filterItems["publishedAt"] = true;
	filterItems["channelId"] = true;
	filterItems["channelTitle"] = true;
	filterItems["title"] = true;
	filterItems["description"] = true;
	filterItems["categoryId"] = true;
	filterItems["liveBroadcastContent"] = true;
	filterItems["defaultLanguage"] = true;
	filterItems["defaultAudioLanguage"] = true;
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
		Serial.println("No results found for video id ");
	}
	else{
		JsonObject itemsSnippet = doc["items"][0]["snippet"];

		if(snip.set){
			freeSnippet(&snip);
		}
		int checksum = 0;

		snip.publishedAt = parseUploadDate(itemsSnippet["publishedAt"]);
		snip.categoryId = itemsSnippet["categoryId"].as<int>();

		checksum += allocAndCopy(&snip.channelId, itemsSnippet["channelId"].as<const char*>());
		checksum += allocAndCopy(&snip.title, itemsSnippet["title"].as<const char*>());
		checksum += allocAndCopy(&snip.description, itemsSnippet["description"].as<const char*>());
		checksum += allocAndCopy(&snip.channelTitle, itemsSnippet["channelTitle"].as<const char*>());
		checksum += allocAndCopy(&snip.liveBroadcastContent, itemsSnippet["liveBroadcastContent"].as<const char*>());

		// language informations appears to be optional, so it is being checked if it is in response
		// if not, a placeholder will be set
		if(!itemsSnippet["defaultLanguage"].as<const char*>()){
			checksum += allocAndCopy(&snip.defaultLanguage, "");
		}else{
			checksum += allocAndCopy(&snip.defaultLanguage, itemsSnippet["defaultLanguage"].as<const char*>());
		}

		if(!itemsSnippet["defaultAudioLanguage"].as<const char*>()){
			checksum += allocAndCopy(&snip.defaultAudioLanguage, "");
		}else{
			checksum += allocAndCopy(&snip.defaultAudioLanguage, itemsSnippet["defaultAudioLanguage"].as<const char*>());
		}
			
		if(checksum){
			// don't set snip.set flag in order to avoid false free
			Serial.print("Error reading in response values. Checksum: ");
			Serial.println(checksum);
			snip.set = false;
			wasSuccessful = false;
		}else{
			snip.set = true;
			wasSuccessful = true;
		}
	}

	closeClient();
	return wasSuccessful;
}


bool YoutubeApi::parseVideoStatus(){

	bool wasSuccessful = false;
	const size_t bufferSize = 384;

	// Creating a filter to filter out 
	// metadata, thumbnail links, tags, localized information

	StaticJsonDocument<192> filter;

	JsonObject filterItems = filter["items"][0].createNestedObject("status");
	filterItems["uploadStatus"] = true;
	filterItems["privacyStatus"] = true;
	filterItems["license"] = true;
	filterItems["embeddable"] = true;
	filterItems["publicStatsViewable"] = true;
	filterItems["madeForKids"] = true;

	JsonObject filterPageInfo = filter.createNestedObject("pageInfo");
	filterPageInfo["totalResults"] = true;
	filterPageInfo["resultsPerPage"] = true;

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
		Serial.println("No results found for video id ");
	}
	else{
		JsonObject itemsStatus = doc["items"][0]["status"];

		if(vStatus.set){
			freeStatus(&vStatus);
		}

		int checksum = 0;
		checksum += allocAndCopy(&vStatus.uploadStatus, itemsStatus["uploadStatus"]);
		checksum += allocAndCopy(&vStatus.privacyStatus, itemsStatus["privacyStatus"]);
		checksum += allocAndCopy(&vStatus.license, itemsStatus["license"]);
		
		vStatus.embeddable = itemsStatus["embeddable"]; // true
		vStatus.publicStatsViewable = itemsStatus["publicStatsViewable"]; // true
		vStatus.madeForKids = itemsStatus["madeForKids"];
		
		if(checksum){
			// don't set videoStatus.set flag in order to avoid false free
			Serial.print("Error reading in response values. Checksum: ");
			Serial.println(checksum);
			vStatus.set = false;
			wasSuccessful = false;
		}else{
			vStatus.set = true;
			wasSuccessful = true;
		}
	}

	closeClient();
	return wasSuccessful;
}


/**
 * @brief Makes an API request for a specific endpoint and type. Calls a parsing function
 * to handle parsing.
 * 
 * @param op API request type to make
 * @param id video or channel id
 * @return Returns parsing function return value, or false in case of an unexpected HTTP status code.
 */
bool YoutubeApi::getRequestedType(int op, const char *id) {

	char command[150];
	bool wasSuccessful = false;
	int httpStatus;

	switch (op)
	{
	case videoListStats:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, "statistics", id, apiKey.c_str());	
		break;
	
	case videoListContentDetails:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, "contentDetails", id, apiKey.c_str());
		break;
	
	case videoListSnippet:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, "snippet", id, apiKey.c_str());
		break;

	case videoListStatus:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, "status", id, apiKey.c_str());
		break;

	case channelListStats:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_CHANNEL_ENDPOINT, "statistics", id, apiKey.c_str());
		break;
	
	default:
		Serial.println("Unknown operation");
		return false;
	}

	if (_debug){
		Serial.println(command);
	}

	httpStatus = sendGetToYoutube(command);

	if (httpStatus == 200)
	{
		switch(op)
		{
			case channelListStats:
				wasSuccessful = parseChannelStatistics();
				break;
			
			case videoListStats:
				wasSuccessful = parseVideoStatistics();
				break;

			case videoListContentDetails:
				wasSuccessful = parseContentDetails();
				break;
			
			case videoListSnippet:
				wasSuccessful = parseSnippet();
				break;

			case videoListStatus:
				wasSuccessful = parseVideoStatus();
				break;

			default:
				wasSuccessful = false;
				break;
		}
	} else {
		Serial.print("Unexpected HTTP Status Code: ");
		Serial.println(httpStatus);
	}
	return wasSuccessful;
}


/**
 * @brief Gets the statistics of a specific channel. Stores them in the calling object.
 * 
 * @param channelId channelID of the channel to get the information from
 * @return true, if there were no errors and the channel was found
 */
bool YoutubeApi::getChannelStatistics(const String& channelId) {
	return getRequestedType(channelListStats, channelId.c_str());
}


bool YoutubeApi::getChannelStatistics(const char *channelId) {
	return getRequestedType(channelListStats, channelId);
}


/**
 * @brief Gets the statistics of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return true, if there were no errors and the video was found
 */
bool YoutubeApi::getVideoStatistics(const String& videoId){
	return getRequestedType(videoListStats, videoId.c_str());
}


bool YoutubeApi::getVideoStatistics(const char *videoId){
	return getRequestedType(videoListStats, videoId);
}


/**
 * @brief Gets the content details of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return true, if there were no errors and the video was found
 */
bool YoutubeApi::getContentDetails(const String& videoId){
	return getRequestedType(videoListContentDetails, videoId.c_str());
}


bool YoutubeApi::getContentDetails(const char *videoId){
	return getRequestedType(videoListContentDetails, videoId);
}


/**
 * @brief Gets the snippet of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return wasSuccesssful true, if there were no errors and the video was found
 */
bool YoutubeApi::getSnippet(const String& videoId){
	return getRequestedType(videoListSnippet, videoId.c_str());
}


bool YoutubeApi::getSnippet(const char *videoId){
	return getRequestedType(videoListSnippet, videoId);
}


/**
 * @brief Gets the status of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return wasSuccesssful true, if there were no errors and the video was found
 */
bool YoutubeApi::getVideoStatus(const String& videoId){
	return getRequestedType(videoListStatus, videoId.c_str());
}


bool YoutubeApi::getVideoStatus(const char *videoId){
	return getRequestedType(videoListStatus, videoId);
}



/**
 * @brief Parses the ISO8601 duration string into a tm time struct.
 * 
 * @param duration Pointer to string
 * @return tm time struct corresponding to duration. When sucessful, it's non zero.
 */
tm YoutubeApi::parseDuration(const char *duration){
	// FIXME
	// rewrite this with strtok?
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
 * @brief Parses the ISO8601 date time string into a tm time struct.
 * 
 * @param dateTime Pointer to string
 * @return tm time struct corresponding to specified time. When sucessful, it's non zero.
 */
tm YoutubeApi::parseUploadDate(const char* dateTime){

	tm ret;
	memset(&ret, 0, sizeof(tm));

	if(!dateTime){
		return ret;
	}

  	int checksum = sscanf(dateTime, "%4d-%2d-%2dT%2d:%2d:%2dZ", &ret.tm_year, &ret.tm_mon, &ret.tm_mday, 
	  															&ret.tm_hour, &ret.tm_min, &ret.tm_sec);

    if(checksum != 6){
        printf("sscanf didn't scan in correctly\n");
		memset(&ret, 0, sizeof(tm));
        return ret;
      }

	ret.tm_year -= 1900;
  
    return ret;
}


/**
 * @brief Frees memory used by strings in snippet struct. Initializes it with zeros.
 * 
 * @param s Pointer to snippet struct to free
 */
void YoutubeApi::freeSnippet(snippet *s){

	if(!s->set){
		return;
	}

	free(s->channelId);
	free(s->title);
	free(s->description);
	free(s->channelTitle);
	free(s->liveBroadcastContent);
	free(s->defaultLanguage);
	free(s->defaultAudioLanguage);

	memset(s, 0, sizeof(snippet));
	s->set = false;

	return;
}


void YoutubeApi::freeStatus(videoStatus *s){

	if(!s->set){
		return;
	}

	free(s->uploadStatus);
	free(s->license);
	free(s->privacyStatus);

	memset(s, 0, sizeof(videoStatus));

	return;
}

/**
 * @brief Allocates memory and copies a string into it.
 * 
 * @param pos where to store a pointer of the allocated memory to
 * @param data pointer of data to copy
 * @return int 0 on success, 1 on error
 */
int YoutubeApi::allocAndCopy(char **pos, const char *data){

	if(!data){
		Serial.println("data is a NULL pointer!");
		return 1;
	}

	size_t size = strlen(data) + 1;
	char *space = (char*) malloc(size);

	if(!space){
		Serial.println("malloc returned NULL pointer!");
		return 1;
	}

	*pos = space;

	memcpy(space, data, size);
	space[size - 1] = '\0';

	return 0;
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
