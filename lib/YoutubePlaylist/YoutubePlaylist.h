#include "YoutubeApi.h"
#include "YoutubeTypes.h"


class YoutubePlaylist{

    public:
        YoutubePlaylist(YoutubeApi *newApiObj, const char *newPlaylistId);
        YoutubePlaylist(YoutubeApi *newApiObj, String& newPlaylistId): YoutubePlaylist(newApiObj, newPlaylistId.c_str()) {};

        ~YoutubePlaylist();

        bool checkPlaylistStatusSet();
        bool checkPlaylistSnipSet();
        bool checkPlaylistContentDetsSet();

        bool checkItemsConfigSet();
        bool checkItemsContentDetsSet();

        const char* getPlaylistId();

        bool getPlaylistStatus();
        bool getPlaylistContentDetails();
        bool getPlaylistSnippet();

        bool getPlaylistItemsPage(int pageNum);


        YoutubeApi* getYoutubeApiObj();

        playlistSnippet *snip = NULL;
        playlistContentDetails *contentDets = NULL;
        playlistStatus *status = NULL;

        // "caches" a page of playlistItems
        playlistItemsContentDetails itemsContentDets[YT_PLAYLIST_ITEM_RESULTS_PER_PAGE];
        
        playlistItemsConfiguration *playlistItemsConfig = NULL;
    private:

        char playlistId[YT_PLAYLISTID_LEN + 1];
        YoutubeApi *apiObj = NULL;

        bool snipSet = false;
        bool contentDetsSet = false;
        bool statusSet = false;
        bool itemsConfigSet = false;
        bool itemsContentDetsSet = false;

        void freePlaylistSnippet();
        void freePlaylistContentDetails();
        void freePlaylistStatus();
        void freePlaylistItemsConfig();

        bool parsePlaylistStatus();
        bool parsePlaylistContentDetails();
        bool parsePlaylistSnippet();

        void setConfig();

        bool getPlaylistItemsContentDetails(bool usePageToken, char *pageToken);

        bool getNextPlaylistItemsPage();
        bool getPreviousPlaylistItemsPage();

        bool parsePlaylistItemsContentDetails();       
};
