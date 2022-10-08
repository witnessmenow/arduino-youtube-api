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
// add	custom error types

#include "YoutubeApi.h"

char YoutubeApi::apiKey[YTAPI_KEY_LEN + 1] = "";

YoutubeApi::YoutubeApi(const char* key, Client &client) : client(client)
{
	strncpy(apiKey, key, YTAPI_KEY_LEN);
	apiKey[YTAPI_KEY_LEN] = '\0';
}

YoutubeApi::YoutubeApi(const String &key, Client &newClient) 
	: YoutubeApi(key.c_str(), newClient)  // passing the key as c-string to force a copy
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
 * @brief Creates a url-string to request data from.
 * 
 * @param mode The type of request to make a string for. (operation enum)
 * @param command The destination of the string.
 * @param id The id of the resource.
 * @return true on success, false on error
 */
bool YoutubeApi::createRequestString(int mode, char* command, const char *id) {

	switch (mode)
	{
	case videoListStats:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, YTAPI_PART_STATISTICS, id, apiKey);	
		break;
	
	case videoListContentDetails:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, YTAPI_PART_CONTENTDETAILS, id, apiKey);
		break;
	
	case videoListSnippet:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, YTAPI_PART_SNIPPET, id, apiKey);
		break;

	case videoListStatus:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_VIDEO_ENDPOINT, YTAPI_PART_STATUS, id, apiKey);
		break;

	case channelListStats:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_CHANNEL_ENDPOINT, YTAPI_PART_STATISTICS, id, apiKey);
		break;

	case channelListSnippet:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_CHANNEL_ENDPOINT, YTAPI_PART_SNIPPET, id, apiKey);
		break;
	
	case channelListContentDetails:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_CHANNEL_ENDPOINT, YTAPI_PART_CONTENTDETAILS, id, apiKey);
		break;

	case playlistListStatus:
		sprintf(command, YTAPI_REQUEST_FORMAT, YTAPI_PLAYLIST_ENDPOINT, YTAPI_PART_STATUS, id, apiKey);
		break;
	
	default:
		Serial.println("Unknown operation");
		return false;
	}

	return true;	
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

	if(!pos){
		Serial.println("pos is a NULL pointer!");
		return 1;
	}

	size_t sizeOfData = strlen(data) + 1;
	char *allocatedMemory = (char*) malloc(sizeOfData);

	if(!allocatedMemory){
		Serial.println("malloc returned NULL pointer!");
		return 1;
	}

	*pos = allocatedMemory;

	memcpy(allocatedMemory, data, sizeOfData);
	allocatedMemory[sizeOfData - 1] = '\0';

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

bool YoutubeApi::checkEmptyResponse(DynamicJsonDocument response){

	if(response["pageInfo"]["totalResults"].as<int>() == 0){
		return true;
	}

	return false;
}


void YoutubeApi::closeClient() {
	if(client.connected()) {
		client.flush();
		client.stop();
	}
}
