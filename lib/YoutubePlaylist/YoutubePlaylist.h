#include "YoutubeApi.h"
#include "YoutubeTypes.h"


class YoutubePlaylist{

    public:
        YoutubePlaylist(YoutubeApi *newApiObj, const char *newPlaylistId);
        YoutubePlaylist(YoutubeApi *newApiObj, String& newPlaylistId): YoutubePlaylist(newApiObj, newPlaylistId.c_str()) {};

        ~YoutubePlaylist();
    
    private:

        char playlistId[YT_PLAYLISTID_LEN + 1];
        YoutubeApi *apiObj;

        playlistSnippet *snip = NULL;
        playlistContentDetails *contentDets = NULL;
        playlistStatus *status = NULL;

        bool snipSet = false;
        bool contentDetsSet = false;
        bool statusSet = false;

        void freePlaylistSnippet();
        void freePlaylistContentDetails();
        void freePlaylistStatus();
};
