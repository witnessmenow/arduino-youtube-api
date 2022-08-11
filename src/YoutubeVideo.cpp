#include <YoutubeVideo.h>
#include <YoutubeApi.h>


YoutubeVideo::YoutubeVideo(const char *newVideoId){
    
    if(videoId == NULL){
        return;
    }

    setVideoId(newVideoId);
}

void YoutubeVideo::setVideoId(const char *newVideoId){
    
    strncpy(videoId, newVideoId, YT_VIDEOID_LEN);
    videoId[YT_VIDEOID_LEN] = '\0';
    videoIdSet = true;
}

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

    memcpy(videoId, 0, YT_VIDEOID_LEN + 1);
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


const char* YoutubeVideo::getVideoId(){
    return videoId;
}

String YoutubeVideo::getVideoIdString(){
    return String(videoId);
}

bool YoutubeVideo::resetVideoId(const char *newVideoId){
    resetInfo();

    if(newVideoId == NULL){
        return false;
    }

    setVideoId(newVideoId);
    return true;
}

bool YoutubeVideo::resetVideoId(String& newVideoId){
    return resetVideoId(newVideoId.c_str());
}


/**
 * @brief Frees memory used by strings in videoSnippet struct. Initializes it with zeros.
 * 
 * @param s Pointer to videoSnippet struct to free
 */
void YoutubeApi::freeVideoSnippet(videoSnippet *s){

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