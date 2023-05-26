#include "YoutubePlaylist.h"


YoutubePlaylist::YoutubePlaylist(YoutubeApi *newApiObj, const char *newPlaylistId){
    strncpy(playlistId, newPlaylistId, YT_PLAYLISTID_LEN);
    playlistId[YT_PLAYLISTID_LEN] = '\0';

    apiObj = newApiObj;
}


YoutubePlaylist::~YoutubePlaylist(){

    freePlaylistSnippet();
    freePlaylistContentDetails();
    freePlaylistStatus();
    freePlaylistItemsConfig();
}


/**
 * @brief Frees playlistSnippet struct of object and resets flag and pointer.
 * 
 */
void YoutubePlaylist::freePlaylistSnippet(){

    if(!snipSet){
        return;
    }

    free(snip->channelId);
    free(snip->title);
    free(snip->description);
    free(snip->channelTitle);

    free(snip);
    snipSet = false;
    snip = NULL;
}


/**
 * @brief Frees playlistContentDetails struct of object and resets flag and pointer.
 * 
 */
void YoutubePlaylist::freePlaylistContentDetails(){

    if(!contentDetsSet){
        return;
    }

    free(contentDets);
    contentDetsSet = false;
    contentDets = NULL;
}


/**
 * @brief Frees playlistStatus struct of object and resets flag and pointer.
 * 
 */
void YoutubePlaylist::freePlaylistStatus(){

    if(!statusSet){
        return;
    }

    free(status->privacyStatus);

    free(status);
    statusSet = false;
    status = NULL;
}


/**
 * @brief Frees playlistItemsConfiguration struct of object and resets flag and pointer.
 * 
 */
void YoutubePlaylist::freePlaylistItemsConfig(){

    if(!itemsConfigSet){
        return;
    }

    free(playlistItemsConfig);
    itemsConfigSet = false;
    playlistItemsConfig = NULL;
}


/**
 * @brief Returns the value of the playlistStatusSet flag, indicating a valid object.
 * 
 * @return Value of flag.
 */
bool YoutubePlaylist::checkPlaylistStatusSet(){return statusSet;}


/**
 * @brief Returns the value of the playlistSnipSet flag, indicating a valid object.
 * 
 * @return Value of flag.
 */
bool YoutubePlaylist::checkPlaylistSnipSet(){return snipSet;}


/**
 * @brief Returns the value of the itemsConfigSet flag, indicating a valid object.
 * 
 * @return Value of flag.
 */
bool YoutubePlaylist::checkPlaylistContentDetsSet(){return contentDetsSet;}


/**
 * @brief Returns the value of the itemsConfigSet flag, indicating a valid connfiguration.
 * 
 * @return Value of flag.
 */
bool YoutubePlaylist::checkItemsConfigSet(){return itemsConfigSet;}


/**
 * @brief Returns the value of the itemsContentDetsSet flag, indicating a valid object.
 * 
 * @return Value of flag.
 */
bool YoutubePlaylist::checkItemsContentDetsSet(){return itemsContentDetsSet;}


/**
 * @brief Returns the currently set playlist id.
 * 
 * @return const char* playlistId
 */
const char* YoutubePlaylist::getPlaylistId(){return playlistId;}


/**
 * @brief Returns the YoutubeApi object of the object.
 * 
 * @return pointer to YoutubeApi object
 */
YoutubeApi* YoutubePlaylist::getYoutubeApiObj(){return apiObj;}


/**
 * @brief  Fetches playlist status of the set playlist id.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::getPlaylistStatus(){

    freePlaylistStatus();

    char command[150];
    YoutubeApi::createRequestString(playlistListStatus, command, playlistId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parsePlaylistStatus();
    }

    return false;
}

/**
 * @brief Parses the response of the api request to retrieve the playlistStatus.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::parsePlaylistStatus(){

    bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 512;
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        if(YoutubeApi::checkEmptyResponse(doc)){
            Serial.println("Could not find playlistId!");
            apiObj->closeClient();
	        return wasSuccessful;
        }

        playlistStatus *newplaylistStatus = (playlistStatus*) malloc(sizeof(playlistStatus));

        uint8_t errorCode = 0;

        errorCode += YoutubeApi::allocAndCopy(&newplaylistStatus->privacyStatus, doc["items"][0]["status"]["privacyStatus"].as<const char*>());
        
        if(errorCode){
            Serial.print("Error code: ");
            Serial.print(errorCode);
        }else{
            status = newplaylistStatus;
            statusSet = true;

		    wasSuccessful = true;
        }
    }
	else{
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}

    apiObj->closeClient();
    return wasSuccessful;
}


/**
 * @brief  Fetches playlist content details of the set playlist id.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::getPlaylistContentDetails(){

    freePlaylistContentDetails();

    char command[150];
    YoutubeApi::createRequestString(playlistListContentDetails, command, playlistId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parsePlaylistContentDetails();
    }

    return false;
}

/**
 * @brief Parses the response of the api request to retrieve the playlist content details.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::parsePlaylistContentDetails(){

    bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 512; // recommended 384, but it throwed errors
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        if(YoutubeApi::checkEmptyResponse(doc)){
            Serial.println("Could not find playlistId!");
            apiObj->closeClient();
	        return wasSuccessful;
        }

        playlistContentDetails *newPlaylistContentDetails = (playlistContentDetails*) malloc(sizeof(playlistContentDetails));

        newPlaylistContentDetails->itemCount = doc["items"][0]["contentDetails"]["itemCount"].as<uint32_t>();
      
        contentDets = newPlaylistContentDetails;
        contentDetsSet = true;
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
 * @brief  Fetches playlist snippet of the set playlist id.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::getPlaylistSnippet(){

    freePlaylistSnippet();

    char command[150];
    YoutubeApi::createRequestString(playlistListSnippet, command, playlistId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parsePlaylistSnippet();
    }

    return false;
}


/**
 * @brief Parses the response of the api request to retrieve the playlist content details.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::parsePlaylistSnippet(){

    bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 1600;     // is just enough for upload playlists. It might not work for user made playlists.
                                        // 1600 Bytes is way too large. #TODO: implement filtering to reduce allocated space
	DynamicJsonDocument doc(bufferSize);

	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        if(YoutubeApi::checkEmptyResponse(doc)){
            Serial.println("Could not find playlistId!");
            apiObj->closeClient();
	        return wasSuccessful;
        }

        playlistSnippet *newPlaylistSnippet = (playlistSnippet*) malloc(sizeof(playlistSnippet));

        uint8_t errorCode = 0;

        errorCode += YoutubeApi::allocAndCopy(&newPlaylistSnippet->channelId, doc["items"][0]["snippet"]["channelId"].as<const char*>());
        errorCode += YoutubeApi::allocAndCopy(&newPlaylistSnippet->title, doc["items"][0]["snippet"]["title"].as<const char*>());
        errorCode += YoutubeApi::allocAndCopy(&newPlaylistSnippet->description, doc["items"][0]["snippet"]["description"].as<const char*>());

        errorCode += YoutubeApi::allocAndCopy(&newPlaylistSnippet->channelTitle, doc["items"][0]["snippet"]["channelTitle"].as<const char*>());

        char *ret = strncpy(newPlaylistSnippet->defaultLanguage, doc["items"][0]["snippet"]["defaultLanguage"].as<const char*>(), 3);
        newPlaylistSnippet->defaultLanguage[3]  = '\0';

        newPlaylistSnippet->publishedAt = YoutubeApi::parseUploadDate(doc["items"][0]["snippet"]["publishedAt"].as<const char*>());

        snip = newPlaylistSnippet;
        snipSet = true;
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
 * @brief Creates and initiliazes an playlistItems configuration object
 * 
 */
void YoutubePlaylist::setConfig(){
    playlistItemsConfiguration *newConfig = (playlistItemsConfiguration*) malloc(sizeof(playlistItemsConfiguration));
    playlistItemsConfig = newConfig;
    playlistItemsConfig->currentPage = 0;
    itemsConfigSet = true;
}

/**
 * @brief Gets a page of playlistItems. A page token can be passed.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::getPlaylistItemsContentDetails(bool usePageToken, char *pageToken){

    char command[180];
    char tokenAndPlaylistId[50];

    if(usePageToken){
        sprintf(tokenAndPlaylistId, "%s&pageToken=%s", playlistId, pageToken);
    }else{
        strcpy(tokenAndPlaylistId, playlistId);
    } 

    YoutubeApi::createRequestString(playlistItemsListContentDetails, command, tokenAndPlaylistId);
    int httpStatus = apiObj->sendGetToYoutube(command);

    if(httpStatus == 200){
        return parsePlaylistItemsContentDetails();

    }

    return false;
}


/**
 * @brief Parses a page of playlistItems:contentDetails. It also modifies values of playlistItemsConfig.
 * 
 * @return true on success, false on error
 */
bool YoutubePlaylist::parsePlaylistItemsContentDetails(){
    bool wasSuccessful = false;

	// Get from https://arduinojson.org/v6/assistant/
	const size_t bufferSize = 2048; 
	DynamicJsonDocument doc(bufferSize);

/*
    Can not get the filter to work - for now.
    It appears the optional parameters (nextPageToken and prevPageToken) break the filter.

    StaticJsonDocument<48> filter;

    filter["nextPageToken"] = true;
    filter["prevPageToken"] = true;
    filter["items"][0]["contentDetails"] = true;
    filter["pageInfo"] = true;
*/
	// Parse JSON object
	DeserializationError error = deserializeJson(doc, apiObj->client);
	if (!error){

        if(YoutubeApi::checkEmptyResponse(doc)){
            Serial.println("Could not find playlistId!");
            apiObj->closeClient();
	        return wasSuccessful;
        }

        uint8_t pos = 0;

        for (JsonObject item : doc["items"].as<JsonArray>()) {

            strcpy(itemsContentDets[pos].videoId ,item["contentDetails"]["videoId"]);
            itemsContentDets[pos].videoPublishedAt = YoutubeApi::parseUploadDate(item["contentDetails"]["videoPublishedAt"]);

            pos++;
        }
        
        playlistItemsConfig->currentPageLastValidPos = pos - 1;

        // if page not full, fill in with dummy data
        if(pos != YT_PLAYLIST_ITEM_RESULTS_PER_PAGE - 1){
            for(int i = pos; i < YT_PLAYLIST_ITEM_RESULTS_PER_PAGE; i++){
                strcpy(itemsContentDets[i].videoId ,"");
                itemsContentDets[i].videoPublishedAt = YoutubeApi::parseUploadDate("1970-01-01T00:00:00Z");
            }
        }

        if(doc["nextPageToken"].as<const char*>()){
            strcpy(playlistItemsConfig->nextPageToken, doc["nextPageToken"].as<const char*>());
        }else{
            strcpy(playlistItemsConfig->nextPageToken, "");
        }

        if(doc["prevPageToken"].as<const char*>()){
            strcpy(playlistItemsConfig->previousPageToken, doc["prevPageToken"].as<const char*>());
        }else{
            strcpy(playlistItemsConfig->previousPageToken, "");
        }
      
        playlistItemsConfig->totalResults = doc["pageInfo"]["totalResults"];

        if(doc["pageInfo"]["resultsPerPage"] != YT_PLAYLIST_ITEM_RESULTS_PER_PAGE){
            Serial.println("WARNING: Unexpected resultsPerPage!");
        }

        itemsContentDetsSet = true;
		wasSuccessful = true;
    }
	else{
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
	}

    apiObj->closeClient();
    return wasSuccessful;
}


bool YoutubePlaylist::getPlaylistItemsPage(int pageNum){

    if(pageNum < 0){
        Serial.println("Page number must be greater than zero!");
        return false;
    }

    if(!playlistItemsConfig || !itemsContentDetsSet){
        // if it is the first time, the object fetches thee page, get the first page first
        setConfig();
        bool ret = getPlaylistItemsContentDetails(false, "");

        if(!ret){return ret;}
    }

    //check if page exists
    if((int) ceil(((float) playlistItemsConfig->totalResults) / YT_PLAYLIST_ITEM_RESULTS_PER_PAGE) < pageNum){
        Serial.println("Page number too large!");
        return false;
    }

    int diff = pageNum - playlistItemsConfig->currentPage;

    // TODO: add skiping logic => sometimes it is faster to skip to the start and traversed from there 
    // TODO: when traversing playlist, contentDetails dont need to be parsed or fetched

    while(diff != 0){
        bool ret;

        if(diff > 0){
            ret = getNextPlaylistItemsPage();
            diff--;
        }else{
            ret = getPreviousPlaylistItemsPage();
            diff++;
        }

        if(!ret){
                Serial.println("Error traversing!");
                return ret;
            }
    }
    return true;    
}


bool YoutubePlaylist::getPreviousPlaylistItemsPage(){
    if(strcmp("", playlistItemsConfig->previousPageToken) == 0){
        Serial.print("There is no previous page!");
        return false;
    }

    char previousPageToken[YT_PLALIST_ITEMS_PAGE_TOKEN_LEN + 1];
    strcpy(previousPageToken, playlistItemsConfig->previousPageToken);

    bool ret = getPlaylistItemsContentDetails(true, previousPageToken);
    if(!ret){return ret;}

    strcpy(playlistItemsConfig->currentPageToken, previousPageToken);
    playlistItemsConfig->currentPage--;

    return true;
}


bool YoutubePlaylist::getNextPlaylistItemsPage(){

    if(strcmp("", playlistItemsConfig->nextPageToken) == 0){
        Serial.print("There is no next page!");
        return false;
    }

    char nextPageToken[YT_PLALIST_ITEMS_PAGE_TOKEN_LEN + 1];
    strcpy(nextPageToken, playlistItemsConfig->nextPageToken);

    bool ret = getPlaylistItemsContentDetails(true, nextPageToken);
    if(!ret){return ret;}

    strcpy(playlistItemsConfig->currentPageToken, nextPageToken);
    playlistItemsConfig->currentPage++;

    return true;
}
