#ifndef YT_TYPES
#define YT_TYPES

#include <ctime>
#include <inttypes.h>

#define YT_VIDEOID_LEN 11
#define YT_CHANNELID_LEN 24
#define YT_PLAYLISTID_LEN 24
#define YT_PLALIST_ITEMS_PAGE_TOKEN_LEN 14
#define YT_PLAYLIST_ITEM_RESULTS_PER_PAGE 5

#define YTAPI_HOST "www.googleapis.com"
#define YTAPI_SSL_PORT 443
#define YTAPI_TIMEOUT 1500

#define YTAPI_CHANNEL_ENDPOINT "/youtube/v3/channels"
#define YTAPI_VIDEO_ENDPOINT "/youtube/v3/videos"
#define YTAPI_PLAYLIST_ENDPOINT "/youtube/v3/playlists"
#define YTAPI_PLAYLIST_ITEMS_ENDPOINT "/youtube/v3/playlistItems"

#define YTAPI_REQUEST_FORMAT "%s?part=%s&id=%s&key=%s"
#define YTAPI_PLAYLIST_ITEMS_REQUEST_FORMAT "%s?part=%s&playlistId=%s&key=%s"

#define YTAPI_PART_STATISTICS "statistics"
#define YTAPI_PART_CONTENTDETAILS "contentDetails"
#define YTAPI_PART_SNIPPET "snippet"
#define YTAPI_PART_STATUS "status"

#define YTAPI_KEY_LEN 45

enum operation{

	videoListStats,
	videoListContentDetails,
	videoListSnippet,
	videoListStatus,

	channelListStats,
	channelListSnippet,
	channelListContentDetails,

	playlistListStatus,
	playlistListContentDetails,
	playlistListSnippet,

	playlistItemsListContentDetails
};


// not implemented data fields are commented out

struct playlistItemsConfiguration{
	uint16_t totalResults;
// 	uint8_t resultsPerPage; should be YT_PLAYLIST_ITEM_RESULTS_PER_PAGE 

	uint16_t currentPage;
	uint8_t currentPageLastValidPos; // last valid data entry on page
	char currentPageToken[YT_PLALIST_ITEMS_PAGE_TOKEN_LEN + 1] = "";

	char nextPageToken[YT_PLALIST_ITEMS_PAGE_TOKEN_LEN + 1] = "";
	char previousPageToken[YT_PLALIST_ITEMS_PAGE_TOKEN_LEN + 1] = "";
};

struct playlistItemsContentDetails{
	char videoId[YT_VIDEOID_LEN + 1] = "";
	tm videoPublishedAt;
};

struct playlistContentDetails{
	uint32_t itemCount;
};

struct playlistSnippet{
	tm publishedAt;
	char *channelId;
	char *title;
	char *description;
//	char **thumbnails;
	char *channelTitle;
	char defaultLanguage[4];
//	char **localized;

};

struct playlistStatus{
	char *privacyStatus;
};


struct channelStatistics {
	uint64_t viewCount;
//	long commentCount;  /* DEPRECATED */
	uint64_t subscriberCount;
	bool hiddenSubscriberCount;
	uint32_t videoCount;
};

struct channelSnippet {
	char *title;
	char *description;
	// char *customUrl;
	tm publishedAt;
	//char **thumbnails;
	char *country;
};

struct channelContentDetails{

	char* relatedPlaylistsLikes;
	char* relatedPlaylistsUploads;
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
	uint64_t viewCount; // uint64_t required for popular videos. (Baby Shark would else overflow xD)
	uint32_t commentCount;
	uint32_t likeCount;
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
