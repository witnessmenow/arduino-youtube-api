#include "YoutubeVideo.h"

YoutubeVideo::YoutubeVideo(const char *newVideoId, YoutubeApi *obj){
    
    if(videoId == NULL){
        return;
    }
    apiObj = obj;
    setVideoId(newVideoId);
}

YoutubeVideo::YoutubeVideo(): apiObj(){}

/**
 * @brief Sets the new videoId. Sets videoIdSet on success
 * 
 * @param newVideoId new videoId to set
 * @return true If the id is valid (len = YT_VIDEOID_LEN)
 * @return false If the id is not valid or a null pointer.
 */
bool YoutubeVideo::setVideoId(const char *newVideoId){

    if(!newVideoId || strlen(newVideoId) != YT_VIDEOID_LEN){
        return false;
    }
    
    strncpy(videoId, newVideoId, YT_VIDEOID_LEN);
    videoId[YT_VIDEOID_LEN] = '\0';
    videoIdSet = true;

    return true;
}

/**
 * @brief Deletes all information from object, except the YoutubeApi object.
 * 
 */
void YoutubeVideo::resetInfo(){

    if(videoSnipSet){
        freeVideoSnippet(videoSnip);
        videoSnip = NULL;
        videoSnipSet = false;
    }

    if(vStatusSet){
        freeVideoStatus(vStatus);
        vStatus = NULL;
        vStatusSet = false;
    }

    if(videoStatsSet){
        free(videoStats);
        videoStats = NULL;
        videoStatsSet = false;
    }

    if(videoContentDetsSet){
        free(videoContentDets);
        videoContentDets = NULL;
        videoContentDetsSet = false;
    }

    strncpy(videoId, "", YT_VIDEOID_LEN + 1);
    videoIdSet = false;
}


YoutubeVideo::~YoutubeVideo(){
    resetInfo();
}


/**
 * @brief Frees memory used by strings in videoStatus struct. Initialzes it with zeroes.
 * 
 * @param s Pointer to videoStatus struct to free
 */
void YoutubeVideo::freeVideoStatus(videoStatus *s){

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
 * @brief Getter function to check if videoId is set.
 * 
 * @return true if it is set, false otherwise
 */
bool YoutubeVideo::checkVideoIdSet(){
    return videoIdSet;
}

/**
 * @brief Getter function to check if videoSnip is set.
 * 
 * @return true if it is set, false otherwise
 */
bool YoutubeVideo::checkVideoSnippetSet() {return videoSnipSet;};

/**
 * @brief Getter function to check if videoStats is set.
 * 
 * @return true if it is set, false otherwise
 */
bool YoutubeVideo::checkVideoStatisticsSet(){return videoStatsSet;};

/**
 * @brief Getter function to check if videoContentDets is set.
 * 
 * @return true if it is set, false otherwise
 */
bool YoutubeVideo::checkVideoContentDetailsSet(){return videoContentDetsSet;};

/**
 * @brief Getter function to check if vStatus is set.
 * 
 * @return true if it is set, false otherwise
 */
bool YoutubeVideo::checkVideoStatusSet(){return vStatusSet;};

/**
 * @brief Getter function to get the value of the current videoId.
 * 
 * @return returns a pointer to the videoId. Might be NULL.
 */
const char* YoutubeVideo::getVideoId(){
    return videoId;
}

YoutubeApi* YoutubeVideo::getYoutubeApiObj(){
	return apiObj;
}

/**
 * @brief Getter function to get the value of the current videoId as String.
 * 
 * @return returns a pointer to the videoId. Might be NULL.
 */
String YoutubeVideo::getVideoIdString(){
    return String(videoId);
}

/**
 * @brief Resets the videoId and all information in the object. 
 *  Even if the id is not valid, all information gets deleted.
 * 
 * @param newVideoId new video id to set
 * @return bool true on success, false if setting the id fails
 */
bool YoutubeVideo::resetVideoId(const char *newVideoId){

    resetInfo();

    if(newVideoId == NULL){
        return false;
    }
    return setVideoId(newVideoId);
}

bool YoutubeVideo::resetVideoId(String& newVideoId){
    return resetVideoId(newVideoId.c_str());
}


/**
 * @brief Frees memory used by strings in videoSnippet struct. Initializes it with zeros.
 * 
 * @param s Pointer to videoSnippet struct to free
 */
void YoutubeVideo::freeVideoSnippet(videoSnippet *s){

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

	memset(s, 0, sizeof(videoSnippet));
	s->set = false;

	return;
}


/**
 * @brief Parses the video statistics from client in YoutubeApi object. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeVideo::parseVideoStatistics(){
	
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
	DeserializationError error = deserializeJson(doc, apiObj->client);

	if (error){
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}
	else if(doc["pageInfo"]["totalResults"].as<int>() == 0){
		Serial.println("No results found for video id ");
	}
	else{

        //TODO check if videoStat already set

        videoStatistics *newStats = (videoStatistics*) malloc(sizeof(videoStatistics)); 

		JsonObject itemStatistics = doc["items"][0]["statistics"];

		newStats->viewCount = itemStatistics["viewCount"].as<long>();
		newStats->likeCount = itemStatistics["likeCount"].as<long>();
		newStats->commentCount= itemStatistics["commentCount"].as<long>();

        videoStats = newStats;
		wasSuccessful = true;
        videoStatsSet = true;
	}

	apiObj->closeClient();

	return wasSuccessful;
}

bool YoutubeVideo::getVideoStatistics(){

    char command[150];
    YoutubeApi::createRequestString(videoListStats, command, videoId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseVideoStatistics();
    }

    return false;
}

/**
 * @brief Gets the snippet of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return wasSuccesssful true, if there were no errors and the video was found
 */
bool YoutubeVideo::getVideoSnippet(){

    char command[150];
    YoutubeApi::createRequestString(videoListSnippet, command, videoId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseVideoSnippet();
    }

    return false;
}

/**
 * @brief Parses the video snippet from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeVideo::parseVideoSnippet(){

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
	DeserializationError error = deserializeJson(doc, apiObj->client, DeserializationOption::Filter(filter));

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

		if(videoSnipSet){
			freeVideoSnippet(videoSnip);
		}

        videoSnippet *newSnippet = (videoSnippet*) malloc(sizeof(videoSnippet)); 

		int checksum = 0;

		newSnippet->publishedAt = YoutubeApi::parseUploadDate(itemsSnippet["publishedAt"]);
		newSnippet->categoryId = itemsSnippet["categoryId"].as<int>();

		checksum += YoutubeApi::allocAndCopy(&newSnippet->channelId, itemsSnippet["channelId"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->title, itemsSnippet["title"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->description, itemsSnippet["description"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->channelTitle, itemsSnippet["channelTitle"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->liveBroadcastContent, itemsSnippet["liveBroadcastContent"].as<const char*>());

		// language informations appears to be optional, so it is being checked if it is in response
		// if not, a placeholder will be set
		if(!itemsSnippet["defaultLanguage"].as<const char*>()){
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultLanguage, "");
		}else{
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultLanguage, itemsSnippet["defaultLanguage"].as<const char*>());
		}

		if(!itemsSnippet["defaultAudioLanguage"].as<const char*>()){
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultAudioLanguage, "");
		}else{
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultAudioLanguage, itemsSnippet["defaultAudioLanguage"].as<const char*>());
		}
			
		if(checksum){
			// don't set snip.set flag in order to avoid false free
			Serial.print("Error reading in response values. Checksum: ");
			Serial.println(checksum);
			videoSnipSet = false;
			wasSuccessful = false;
		}else{
			videoSnipSet = true;
			videoSnip = newSnippet;
			wasSuccessful = true;
		}
	}

	apiObj->closeClient();
	delay(20);
	return wasSuccessful;
}
