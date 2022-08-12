#include "YoutubeVideo.h"

YoutubeVideo::YoutubeVideo(const char *newVideoId){
    
    if(videoId == NULL){
        return;
    }

    setVideoId(newVideoId);
}

YoutubeVideo::YoutubeVideo(){}

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
 * @brief Deletes all information from object. 
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
 * @brief Get function to check if videoId is set.
 * 
 * @return bool value of videoIdSet
 */
bool YoutubeVideo::checkVideoIdSet(){
    return videoIdSet;
}


const char* YoutubeVideo::getVideoId(){
    return videoId;
}

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

