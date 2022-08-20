#include "YoutubeChannel.h"

YoutubeChannel::YoutubeChannel(const char *newChannelId, YoutubeApi *newApiObj){
    if(newApiObj){
        apiObj = newApiObj;
    }

    setChannelId(newChannelId);
}


void YoutubeChannel::setChannelId(const char *newChannelId){

    if(!newChannelId || strlen(newChannelId) != YT_CHANNELID_LEN){
        return;
    }

    strncpy(channelId, newChannelId, YT_CHANNELID_LEN);
    channelId[YT_CHANNELID_LEN] = '\0';
    channelIdSet = true;
}


const char* YoutubeChannel::getChannelId(){
    return channelId;
}

YoutubeApi* YoutubeChannel::getYoututbeApiObj(){
    return apiObj;
}

void YoutubeChannel::freeChannelStats(){
    if(channelStatsSet && channelStats){
        free(channelStats);
        channelStats = NULL;
        channelStatsSet = false;
    }
}


bool YoutubeChannel::checkChannelIdSet(){ return channelIdSet; }

bool YoutubeChannel::checkChannelStatsSet(){return channelStatsSet;}

void YoutubeChannel::resetInfo(){
    freeChannelStats();

    strncpy(channelId, "", YT_CHANNELID_LEN + 1);
    channelIdSet = false;
}

YoutubeChannel::~YoutubeChannel(){
    resetInfo();
}

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
