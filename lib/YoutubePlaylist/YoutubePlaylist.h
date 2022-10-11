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

        const char* getPlaylistId();

        bool getPlaylistStatus();
        bool getPlaylistContentDetails();
        bool getPlaylistSnippet();

        playlistSnippet *snip = NULL;
        playlistContentDetails *contentDets = NULL;
        playlistStatus *status = NULL;

        // "caches" a page of playlistItems
        playlistItemsContentDetails itemsContentDets[YT_PLAYLIST_ITEM_RESULTS_PER_PAGE];
        
    private:

        char playlistId[YT_PLAYLISTID_LEN + 1];
        YoutubeApi *apiObj = NULL;
        playlistItemsConfiguration *playlistItemsConfig = NULL;

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

        bool getPlaylistItemsInitialConfig();
        bool parsePlaylistItemsContentDetails();
};
