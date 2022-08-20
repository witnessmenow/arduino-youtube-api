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