#ifndef YoutubeVideo_h
#define YoutubeVideo_h

#include <Arduino.h>
#include <YoutubeApi.h>

class YoutubeVideo{

    public: 
        YoutubeVideo(const char *newVideoId);
        YoutubeVideo(String& newVideoId): YoutubeVideo(newVideoId.c_str()) {};
        YoutubeVideo();

        ~YoutubeVideo();

        bool getVideoStatistics();
        bool getVideoSnippet();
        bool getVideoContentDetails();

        bool resetVideoId(const char *newVideoId);
        bool resetVideoId(String& newVideoId);
        void resetInfo();
        const char* getVideoId();
        String getVideoIdString();

        videoSnippet *videoSnip = NULL;
		videoStatistics *videoStats = NULL;
		videoContentDetails *videoContentDets = NULL;
        videoStatus *vStatus = NULL;

    private:

        char videoId[YT_VIDEOID_LEN + 1];

        bool videoIdSet = false;
        bool videoSnipSet = false;
        bool videoStatsSet = false;
        bool videoContentDetsSet = false;
        bool vStatusSet = false;

        void setVideoId(const char *newVideoId);

        void freeVideoSnippet(videoSnippet *s);
		void freeVideoStatus(videoStatus *s);

};



#endif