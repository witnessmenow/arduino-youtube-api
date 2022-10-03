#include "YoutubeChannel.h"

YoutubeChannel::YoutubeChannel(const char *newChannelId, YoutubeApi *newApiObj){
    if(newApiObj){
        apiObj = newApiObj;
    }

    setChannelId(newChannelId);
}

/**
 * @brief Private function to set the channel id.
 * 
 * @param newChannelId new channel id o set
 */
void YoutubeChannel::setChannelId(const char *newChannelId){

    if(!newChannelId || strlen(newChannelId) != YT_CHANNELID_LEN){
        return;
    }

    strncpy(channelId, newChannelId, YT_CHANNELID_LEN);
    channelId[YT_CHANNELID_LEN] = '\0';
    channelIdSet = true;
}

/**
 * @brief Returns the stored channel id.
 * 
 * @return const char* currently stored channel id. If none is stored, returns an empty string
 */
const char* YoutubeChannel::getChannelId(){
    return channelId;
}

/**
 * @brief Returns the current YoutubeApi object used to fetch requests.
 * 
 * @return YoutubeApi* pointer to currrent YoutubeApi object
 */
YoutubeApi* YoutubeChannel::getYoututbeApiObj(){
    return apiObj;
}

/**
 * @brief Deletes channel statistics and resets the flag.
 * 
 */
void YoutubeChannel::freeChannelStats(){
    if(channelStatsSet && channelStats){
        free(channelStats);
        channelStats = NULL;
        channelStatsSet = false;
    }
}

/**
 * @brief Frees channelContentDetails struct of object.
 * 
 */
void YoutubeChannel::freeChannelContentDetails(){
    if(!channelContentDetailsSet){
        return;
    }

    free(channelContentDets->relatedPlaylistsLikes);
    free(channelContentDets->relatedPlaylistsUploads);
    free(channelContentDets);

    channelContentDets = NULL;
    channelContentDetailsSet = false;
}


/**
 * @brief Returns the flag indicating if channel id is currently set (and valid).
 * 
 * @return boolean value of the flag
 */
bool YoutubeChannel::checkChannelIdSet(){ return channelIdSet; }

/**
 * @brief Returns the flag indicating if channel statistics object is currently set (and valid).
 * 
 * @return boolean value of the flag
 */
bool YoutubeChannel::checkChannelStatsSet(){return channelStatsSet;}

/**
 * @brief Returns the flag indicating if channel snippet object is currently set (and valid).
 * 
 * @return boolean value of the flag
 */
bool YoutubeChannel::checkChannelSnipSet(){return channelSnipSet;};


/**
 * @brief Returns the flag indicating if channel content details object is currently set (and valid).
 * 
 * @return boolean value of the flag
 */
bool YoutubeChannel::checkChannelContentDetailsSet(){return channelContentDetailsSet;}

/**
 * @brief Resets all information of the YoutubeChannel object, except the YoutubeApi object.
 * 
 */
void YoutubeChannel::resetInfo(){
    freeChannelStats();
    freeChannelSnippet();
    freeChannelContentDetails();

    strncpy(channelId, "", YT_CHANNELID_LEN + 1);
    channelIdSet = false;
}

YoutubeChannel::~YoutubeChannel(){
    resetInfo();
}

/**
 * @brief Fetches channel statistics of the set channel id.
 * 
 * @return true on success, otherwise false
 */
bool YoutubeChannel::getChannelStatistics(){
    if(channelStatsSet){
        freeChannelStats();
    }

    char command[150];
    YoutubeApi::createRequestString(channelListStats, command, channelId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseChannelStatistics();
    }

    return false;
}


/**
 * @brief Parses the channel statistics from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeChannel::parseChannelStatistics() {
	
	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = JSON_ARRAY_SIZE(1) 
	                        + JSON_OBJECT_SIZE(2) 
	                        + 2*JSON_OBJECT_SIZE(4) 
	                        + JSON_OBJECT_SIZE(5)
	                        + 330;
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        channelStatistics *newChannelStats = (channelStatistics*) malloc(sizeof(channelStatistics));

		JsonObject itemStatistics = doc["items"][0]["statistics"];

		newChannelStats->viewCount = itemStatistics["viewCount"].as<uint64_t>();
		newChannelStats->subscriberCount = itemStatistics["subscriberCount"].as<uint64_t>();
		newChannelStats->hiddenSubscriberCount = itemStatistics["hiddenSubscriberCount"].as<bool>();
		newChannelStats->videoCount = itemStatistics["videoCount"].as<uint32_t>();

        channelStats = newChannelStats;
        channelStatsSet = true;

		wasSuccessful = true;
	}
	else{
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}

	apiObj->closeClient();
	return wasSuccessful;
}



/**
 * @brief Fetches channel statistics of the set channel id.
 * 
 * @return true on success, otherwise false
 */
bool YoutubeChannel::getChannelSnippet(){
    if(channelSnipSet){
        freeChannelSnippet();
    }

    char command[150];
    YoutubeApi::createRequestString(channelListSnippet, command, channelId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseChannelSnippet();
    }

    return false;
}


/**
 * @brief Parses the channel statistics from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeChannel::parseChannelSnippet() {
	
	bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 2000;
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        channelSnippet *newChannelSnippet = (channelSnippet*) malloc(sizeof(channelSnippet));

		JsonObject itemSnippet = doc["items"][0]["snippet"];

        uint8_t errorCode = 0;

        errorCode += YoutubeApi::allocAndCopy(&newChannelSnippet->title, itemSnippet["title"].as<const char*>());
        errorCode += YoutubeApi::allocAndCopy(&newChannelSnippet->description, itemSnippet["description"].as<const char*>());
        errorCode += YoutubeApi::allocAndCopy(&newChannelSnippet->country, itemSnippet["country"].as<const char*>());
        
        newChannelSnippet->publishedAt = YoutubeApi::parseUploadDate(itemSnippet["publishedAt"].as<const char*>());

        if(errorCode){
            Serial.print("Error code: ");
            Serial.print(errorCode);
        }

        channelSnip = newChannelSnippet;
        channelSnipSet = true;

		wasSuccessful = true;
	}
	else{
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}

	apiObj->closeClient();
	return wasSuccessful;
}


/**
 * @brief Frees the channel snippet struct of the object.
 * 
 */
void YoutubeChannel::freeChannelSnippet(){

    if(!channelSnip){
        channelSnipSet = false;
        return;
    }

    free(channelSnip->title);
    free(channelSnip->description);
    free(channelSnip->country);

    free(channelSnip);

    channelSnip = NULL;
    channelSnipSet = false;
}

/**
 * @brief  Fetches channel contentDetails of the set channel id.
 * 
 * @return true on success, false on error
 */
bool YoutubeChannel::getChannelContentDetails(){
    if(channelContentDetailsSet){
        freeChannelContentDetails();
    }

    char command[150];
    YoutubeApi::createRequestString(channelListContentDetails, command, channelId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseChannelContentDetails();
    }

    return false;
}

bool YoutubeChannel::parseChannelContentDetails(){

    bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 600;
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        channelContentDetails *newChannelContentDetails = (channelContentDetails*) malloc(sizeof(channelContentDetails));

		JsonObject itemContentDetails = doc["items"][0]["contentDetails"]["relatedPlaylists"];

        uint8_t errorCode = 0;

        errorCode += YoutubeApi::allocAndCopy(&newChannelContentDetails->relatedPlaylistsLikes, itemContentDetails["likes"].as<const char*>());
        errorCode += YoutubeApi::allocAndCopy(&newChannelContentDetails->relatedPlaylistsUploads, itemContentDetails["uploads"].as<const char*>());
    
        if(errorCode){
            Serial.print("Error code: ");
            Serial.print(errorCode);
        }

        channelContentDets = newChannelContentDetails;
        channelContentDetailsSet = true;

		wasSuccessful = true;
	}
	else{
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}

	apiObj->closeClient();
	return wasSuccessful;
}
