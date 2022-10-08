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
    free(snip->defaultLanguage);

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
 * @brief Returns the value of the playlistContentDets flag, indicating a valid object.
 * 
 * @return Value of flag.
 */
bool YoutubePlaylist::checkPlaylistContentDetsSet(){return contentDetsSet;}


/**
 * @brief Returns the currently set playlist id.
 * 
 * @return const char* playlistId
 */
const char* YoutubePlaylist::getPlaylistId(){return playlistId;}
