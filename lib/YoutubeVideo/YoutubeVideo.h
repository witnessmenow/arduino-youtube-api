#ifndef YoutubeVideo_h
#define YoutubeVideo_h

#include "YoutubeTypes.h"
#include "YoutubeApi.h"
#include <ArduinoJson.h>
#include <Client.h>
#include <Arduino.h>

class YoutubeVideo{

    public:
        YoutubeVideo();
    
        YoutubeVideo(const char *newVideoId, YoutubeApi *apiObj);
        YoutubeVideo(String& newVideoId, YoutubeApi *apiObj): YoutubeVideo(newVideoId.c_str(), apiObj) {};
        
        ~YoutubeVideo();

        bool getVideoStatistics();
        bool getVideoSnippet();
        bool getVideoContentDetails();

        bool checkVideoIdSet();
        bool checkVideoSnippetSet();
        bool checkVideoStatisticsSet();
        bool checkVideoContentDetailsSet();
        bool checkVideoStatusSet();

        bool resetVideoId(const char *newVideoId);
        bool resetVideoId(String& newVideoId);
        void resetInfo();

        const char* getVideoId();
        YoutubeApi* getYoutubeApiObj();
        String getVideoIdString();

        videoSnippet *videoSnip = NULL;
		videoStatistics *videoStats = NULL;
		videoContentDetails *videoContentDets = NULL;
        videoStatus *vStatus = NULL;

    private:

        char videoId[YT_VIDEOID_LEN + 1] = "";

        bool videoIdSet = false;
        bool videoSnipSet = false;
        bool videoStatsSet = false;
        bool videoContentDetsSet = false;
        bool vStatusSet = false;

        YoutubeApi *apiObj;

        bool parseVideoStatistics();
        bool parseVideoSnippet();

        bool setVideoId(const char *newVideoId);

        void freeVideoSnippet(videoSnippet *s);
		void freeVideoStatus(videoStatus *s);

    #ifdef UNIT_TESTING 
        friend void test_StringConstructor_simple() 
    #endif

};



#endif