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
 * @brief Frees memory used by strings in videoStatus struct and the struct itself.
 * 
 * @param s Pointer to videoStatus struct to free
 * 
 * @return true on success, false on error
 */
bool YoutubeVideo::freeVideoStatus(videoStatus *s){

	if(!s){
		return false;
	}

	if(!s->set){
		return false;
	}

	free(s->uploadStatus);
	free(s->license);
	free(s->privacyStatus);

	free(s);

	return true;
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
 * @brief Frees memory used by strings in videoSnippet struct and the struct itself.
 * 
 * @param s Pointer to videoSnippet struct to free
 * @return true on success, false on error
 */
bool YoutubeVideo::freeVideoSnippet(videoSnippet *s){

	if(!s){
		return false;
	}

	if(!s->set){
		return false;
	}

	free(s->channelId);
	free(s->title);
	free(s->description);
	free(s->channelTitle);
	free(s->liveBroadcastContent);
	free(s->defaultLanguage);
	free(s->defaultAudioLanguage);

	free(s);

	return true;
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

	DynamicJsonDocument doc(bufferSize);
	DeserializationError error = deserializeJson(doc, apiObj->client);

	if (error){
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}
	else if(doc["pageInfo"]["totalResults"].as<int>() == 0){
		Serial.println("No results found for video id ");
	}
	else{
        videoStatistics *newStats = (videoStatistics*) malloc(sizeof(videoStatistics)); 

		newStats->viewCount = doc["items"][0]["statistics"]["viewCount"].as<uint64_t>();
		newStats->likeCount = doc["items"][0]["statistics"]["likeCount"].as<uint32_t>();
		newStats->commentCount= doc["items"][0]["statistics"]["commentCount"].as<uint32_t>();

        videoStats = newStats;
		videoStatsSet = true;
		wasSuccessful = true;
	}

	apiObj->closeClient();

	return wasSuccessful;
}

bool YoutubeVideo::getVideoStatistics(){

	if(checkVideoStatisticsSet()){
		free(videoStats);
		videoStatsSet = false;
		videoStats = NULL;
	}

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

	if(checkVideoSnippetSet()){
		if(!freeVideoSnippet(videoSnip)){
			return false;
		}

		videoSnipSet = false;
		videoSnip = NULL;
	}

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
        videoSnippet *newSnippet = (videoSnippet*) malloc(sizeof(videoSnippet)); 

		int checksum = 0;

		newSnippet->publishedAt = YoutubeApi::parseUploadDate(doc["items"][0]["snippet"]["publishedAt"]);
		newSnippet->categoryId = doc["items"][0]["snippet"]["categoryId"].as<int>();

		checksum += YoutubeApi::allocAndCopy(&newSnippet->channelId, doc["items"][0]["snippet"]["channelId"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->title, doc["items"][0]["snippet"]["title"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->description, doc["items"][0]["snippet"]["description"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->channelTitle, doc["items"][0]["snippet"]["channelTitle"].as<const char*>());
		checksum += YoutubeApi::allocAndCopy(&newSnippet->liveBroadcastContent, doc["items"][0]["snippet"]["liveBroadcastContent"].as<const char*>());

		// language informations appears to be optional, so it is being checked if it is in response
		// if not, a placeholder will be set
		if(!doc["items"][0]["snippet"]["defaultLanguage"].as<const char*>()){
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultLanguage, "");
		}else{
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultLanguage, doc["items"][0]["snippet"]["defaultLanguage"].as<const char*>());
		}

		if(!doc["items"][0]["snippet"]["defaultAudioLanguage"].as<const char*>()){
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultAudioLanguage, "");
		}else{
			checksum += YoutubeApi::allocAndCopy(&newSnippet->defaultAudioLanguage, doc["items"][0]["snippet"]["defaultAudioLanguage"].as<const char*>());
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

/**
 * @brief Gets the status of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return wasSuccesssful true, if there were no errors and the video was found
 */
bool YoutubeVideo::getVideoStatus(){
	if(checkVideoStatusSet()){
		if(!freeVideoStatus(vStatus)){
			return false;
		}
		vStatusSet = false;
		vStatus = NULL;
	}

    char command[150];
    YoutubeApi::createRequestString(videoListStatus, command, videoId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseVideoStatus();
    }
    return false;
}


/**
 * @brief Parses the video status from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeVideo::parseVideoStatus(){

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
		JsonObject itemsStatus = doc["items"][0]["status"];

		videoStatus *newVideoStatus = (videoStatus*) malloc(sizeof(videoStatus));

		int checksum = 0;
		checksum += YoutubeApi::allocAndCopy(&newVideoStatus->uploadStatus, itemsStatus["uploadStatus"]);
		checksum += YoutubeApi::allocAndCopy(&newVideoStatus->privacyStatus, itemsStatus["privacyStatus"]);
		checksum += YoutubeApi::allocAndCopy(&newVideoStatus->license, itemsStatus["license"]);
		
		newVideoStatus->embeddable = itemsStatus["embeddable"]; // true
		newVideoStatus->publicStatsViewable = itemsStatus["publicStatsViewable"]; // true
		newVideoStatus->madeForKids = itemsStatus["madeForKids"];
		
		if(checksum){
			// don't set videoStatus.set flag in order to avoid false free
			Serial.print("Error reading in response values. Checksum: ");
			Serial.println(checksum);
			wasSuccessful = false;
		}else{
			vStatusSet = true;
			vStatus = newVideoStatus;
			wasSuccessful = true;
		}
	}

	apiObj->closeClient();
	return wasSuccessful;
}

/**
 * @brief Gets the content details of a specific video. Stores them in the calling object.
 * 
 * @param videoId videoID of the video to get the information from
 * @return true, if there were no errors and the video was found
 */
bool YoutubeVideo::getVideoContentDetails(){
		if(videoContentDetsSet){
			free(videoContentDets);
			videoContentDets = NULL;
			videoContentDetsSet = false;
		}

    char command[150];
    YoutubeApi::createRequestString(videoListContentDetails, command, videoId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parseVideoContentDetails();
    }
    return false;
}


/**
 * @brief Parses the video content details from caller client. Stores information in calling object.
 * 
 * @return true on success, false on error 
 */
bool YoutubeVideo::parseVideoContentDetails(){
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
		videoContentDetails *newContentDetails = (videoContentDetails*) malloc(sizeof(videoContentDetails));

		memcpy(newContentDetails ->defintion, doc["items"][0]["contentDetails"]["definition"].as<const char *>(), 3);
		memcpy(newContentDetails ->dimension, doc["items"][0]["contentDetails"]["dimension"].as<const char *>(), 3);
		strcpy(newContentDetails ->projection, doc["items"][0]["contentDetails"]["projection"].as<const char*>());

		if("false" == doc["items"][0]["contentDetails"]["caption"]){
			newContentDetails ->caption = true;
		}
		else{
			newContentDetails->caption = false;
		}
			
		newContentDetails ->licensedContent = doc["items"][0]["contentDetails"]["licensedContent"].as<bool>();
		newContentDetails ->duration = YoutubeApi::parseDuration(doc["items"][0]["contentDetails"]["duration"].as<const char*>());

		videoContentDets = newContentDetails;
		videoContentDetsSet = true;

		wasSuccessful = true;
	}
		
	apiObj->closeClient();
	return wasSuccessful;
}
