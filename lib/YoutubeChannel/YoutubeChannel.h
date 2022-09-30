#include "YoutubeApi.h"
#include "YoutubeTypes.h"


class YoutubeChannel{

    public:
        YoutubeChannel(YoutubeApi *newApiObj): YoutubeChannel(NULL, newApiObj){};
        YoutubeChannel(const char *newChannelId, YoutubeApi *newApiObj);
        YoutubeChannel(String& newChannelId, YoutubeApi *newApiObj): YoutubeChannel(newChannelId.c_str(), newApiObj) {};

        ~YoutubeChannel();

        channelStatistics *channelStats = NULL;
        channelSnippet *channelSnip = NULL;
        channelContentDetails *channelContentDets = NULL;

        bool checkChannelStatsSet();
        bool checkChannelSnipSet();
        bool checkChannelIdSet();
        bool checkChannelContentDetailsSet();

        YoutubeApi* getYoututbeApiObj();
        const char *getChannelId();
        void resetInfo();

        bool getChannelStatistics();
        bool getChannelSnippet();
        bool getChannelContentDetails();

    private:

        char channelId[YT_CHANNELID_LEN + 1] = "";
        YoutubeApi *apiObj = NULL;
    
        bool channelIdSet = false;
        bool channelStatsSet = false;
        bool channelSnipSet = false;
        bool channelContentDetailsSet = false;

        void freeChannelStats();
        void freeChannelSnippet();
        void freeChannelContentDetails();

        void setChannelId(const char *newChannelId);

        bool parseChannelStatistics();
        bool parseChannelSnippet();
        bool parseChannelContentDetails();
};