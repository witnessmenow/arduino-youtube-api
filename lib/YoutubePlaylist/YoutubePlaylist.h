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

        playlistSnippet *snip = NULL;
        playlistContentDetails *contentDets = NULL;
        playlistStatus *status = NULL;
        
    private:

        char playlistId[YT_PLAYLISTID_LEN + 1];
        YoutubeApi *apiObj;

        bool snipSet = false;
        bool contentDetsSet = false;
        bool statusSet = false;

        void freePlaylistSnippet();
        void freePlaylistContentDetails();
        void freePlaylistStatus();

        bool parsePlaylistStatus();
        bool parsePlaylistContentDetails();
};
