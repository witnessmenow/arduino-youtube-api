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
