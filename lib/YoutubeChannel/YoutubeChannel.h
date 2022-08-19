#include "YoutubeApi.h"
#include "YoutubeTypes.h"


class YoutubeChannel{

    public:
        YoutubeChannel();
        YoutubeChannel(const char *newChannelId, YoutubeApi *newApiObj);
        YoutubeChannel(String& newChannelId, YoutubeApi *newApiObj): YoutubeChannel(newChannelId.c_str(), newApiObj) {};

        ~YoutubeChannel();

        channelStatistics *channelStats = NULL;
        bool checkChannelStatsSet();
        YoutubeApi* getYoututbeApiObj();
        const char *getChannelId();
        void resetInfo();

    private:

        char channelId[YT_CHANNELID_LEN + 1] = "";
        YoutubeApi *apiObj = NULL;
    
        bool channelIdSet = false;
        bool channelStatsSet = false;

        void freeChannelStats();
        void setChannelId(const char *newChannelId);
};