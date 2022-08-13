#ifndef YT_TYPES
#define YT_TYPES

#include <ctime>

#define YT_VIDEOID_LEN 11
#define YTAPI_HOST "www.googleapis.com"
#define YTAPI_SSL_PORT 443
#define YTAPI_TIMEOUT 1500

#define YTAPI_CHANNEL_ENDPOINT "/youtube/v3/channels"
#define YTAPI_VIDEO_ENDPOINT "/youtube/v3/videos"
#define YTAPI_REQUEST_FORMAT "%s?part=%s&id=%s&key=%s"
#define YTAPI_KEY_LEN 45

enum operation{

	videoListStats,
	videoListContentDetails,
	videoListSnippet,
	videoListStatus,

	channelListStats
};


// not implemented data fields are commented

struct channelStatistics {
	long viewCount;
	long commentCount;  /* DEPRECATED */
	long subscriberCount;
	bool hiddenSubscriberCount;
	long videoCount;
};


struct videoContentDetails{
	tm duration;
	char dimension[3];
	char defintion[3];
	bool caption;
	bool licensedContent;
//	char **regionRestriction;
//	char **contentRating;
	char projection[12];
//	bool hasCustomThumbnail;	
};


struct videoStatistics {
	long viewCount;
	long commentCount;
	long likeCount;
//	long favourites;	
//	long dislikeCount;

//	In Memory of the old dislike count.	
};


struct videoStatus{
	bool set;
	char *uploadStatus;
//	char *failureReason;
//	char *rejectionReason;
	char *privacyStatus;
//	tm publishAt;	
	char *license;
	bool embeddable;
	bool publicStatsViewable;
	bool madeForKids;
//	bool selfDeclaredMadeForKids;
};

struct videoSnippet{
	bool set;
	tm publishedAt;
	char *channelId;
	char *title;
	char *description;
//  char **thumbnails;
	char *channelTitle;
//	char **tags;
	int categoryId;
	char *liveBroadcastContent;
	char *defaultLanguage;
//  char **localized;
	char *defaultAudioLanguage;
};

#endif