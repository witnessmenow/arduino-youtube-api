/*
   Copyright (c) 2020 Brian Lough. All right reserved.

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
	int strLen = apiKey.length() + 1; 
	char tempStr[strLen];
	apiKey.toCharArray(tempStr, strLen);

	YoutubeApi(tempStr, client);
}

YoutubeApi::YoutubeApi(char *apiKey, Client &client)	{
	_apiKey = apiKey;
	this->client = &client;
}

int YoutubeApi::sendGetToYoutube(char *command) {
	client->flush();
    client->setTimeout(YTAPI_TIMEOUT);
	if (!client->connect(YTAPI_HOST, YTAPI_SSL_PORT))
    {
        Serial.println(F("Connection failed"));
        return false;
    }
	// give the esp a breather
    yield();

    // Send HTTP request
    client->print(F("GET "));
    client->print(command);
    client->println(F(" HTTP/1.1"));

	//Headers
    client->print(F("Host: "));
    client->println(YTAPI_HOST);

    client->println(F("Cache-Control: no-cache"));

	if (client->println() == 0)
    {
        Serial.println(F("Failed to send request"));
        return -1;
    }

	int statusCode = getHttpStatusCode();
    
    // Let the caller of this method parse the JSon from the client
    skipHeaders();
    return statusCode;
}

bool YoutubeApi::getChannelStatistics(String channelId){

	int strLen = channelId.length() + 1; 
	char tempStr[strLen];
	channelId.toCharArray(tempStr, strLen);

	return getChannelStatistics(tempStr);
}

bool YoutubeApi::getChannelStatistics(char *channelId){
	char command[150] = YTAPI_CHANNEL_ENDPOINT;
    char params[120];
    sprintf(params, "?part=statistics&id=%s&key=%s", channelId, _apiKey);
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
        DeserializationError error = deserializeJson(doc, *client);
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

void YoutubeApi::skipHeaders()
{
    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client->find(endOfHeaders))
    {
        Serial.println(F("Invalid response"));
        return;
    }

    // Was getting stray characters between the headers and the body
    // This should toss them away
    while (client->available() && client->peek() != '{')
    {
        char c = 0;
        client->readBytes(&c, 1);
        if (_debug)
        {
            Serial.print("Tossing an unexpected character: ");
            Serial.println(c);
        }
    }
}

int YoutubeApi::getHttpStatusCode()
{
    // Check HTTP status
    if(client->find("HTTP/1.1")){
        int statusCode = client->parseInt();
        return statusCode;
    } 

    return -1;
}

void YoutubeApi::closeClient() {
	if(client->connected()) {
		if(_debug) { Serial.println(F("Closing client")); }
		client->stop();
	}
}
