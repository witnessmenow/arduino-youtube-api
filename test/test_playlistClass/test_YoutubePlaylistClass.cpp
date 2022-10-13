#include <unity.h>
#include <Arduino.h>
#include "secrets.h"
#include "YoutubePlaylist.h"
#include "YoutubeTypes.h"
#include <WiFiClientSecure.h>


#define MAX_WIFI_RETRIES 10

char playlistId[YT_PLAYLISTID_LEN + 1];

void test_constructDestruct_simple(){

    WiFiClientSecure dummyClient;
    YoutubeApi dummyApi(API_KEY, dummyClient);

    YoutubePlaylist uut(&dummyApi, TEST_PLAYLIST_ID);

    TEST_ASSERT_FALSE_MESSAGE(uut.checkPlaylistContentDetsSet(), "Should be false in initial state!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkPlaylistStatusSet(), "Should be false in initial state!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkPlaylistSnipSet(), "Should be false in initial state!");

    TEST_ASSERT_NULL_MESSAGE(uut.snip, "Should be NULL in initial state!");
    TEST_ASSERT_NULL_MESSAGE(uut.contentDets, "Should be NULL in initial state!");
    TEST_ASSERT_NULL_MESSAGE(uut.status, "Should be NULL in initial state!");
}

void test_getPlaylistStatus_simple(){

     if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    WiFiClientSecure dummyClient;
    YoutubeApi dummyApi(API_KEY, dummyClient);

    YoutubePlaylist uut(&dummyApi, TEST_PLAYLIST_ID);

    dummyClient.setInsecure();

    bool ret = uut.getPlaylistStatus();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to be able to get a playlist status!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.status, "Expected a valid playlistStatus object to be set!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.status->privacyStatus, "Expected a valid string to be set!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkPlaylistStatusSet(), "Expected the playlistStatus flag to be set!");
}


void test_getPlaylistContentDetails_simple(){

     if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    WiFiClientSecure dummyClient;
    YoutubeApi dummyApi(API_KEY, dummyClient);

    YoutubePlaylist uut(&dummyApi, TEST_PLAYLIST_ID);

    dummyClient.setInsecure(); 

    bool ret = uut.getPlaylistContentDetails();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to be able to get a playlist content details!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.contentDets, "Expected a valid playlistContentDetails object to be set!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkPlaylistContentDetsSet(), "Expected the playlistContentDetails flag to be set!");
}


void test_getPlaylistSnippet_simple(){

     if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    WiFiClientSecure dummyClient;
    YoutubeApi dummyApi(API_KEY, dummyClient);

    YoutubePlaylist uut(&dummyApi, TEST_PLAYLIST_ID);

    dummyClient.setInsecure(); 

    bool ret = uut.getPlaylistSnippet();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to be able to get a playlist snippet!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.snip, "Expected a valid playlist snippet object to be set!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkPlaylistSnipSet(), "Expected the pplaylist snippet flag to be set!");

    TEST_ASSERT_NOT_NULL_MESSAGE(uut.snip->channelId, "Expected a valid channel id string to be set!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.snip->title, "Expected a valid title string to be set!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.snip->description, "Expected a description string to be set!");
    TEST_ASSERT_NOT_NULL_MESSAGE(uut.snip->channelTitle, "Expected a valid channel title stringt to be set!");
}


/**
 * @brief Checks if the given playlistItem is valid, or a "filler" set with default values
 * 
 * @param c playlistItem to check
 * @return returns true, if default values were detected. Otherwise returns false
 */
bool checkForDefaultPlaylistItemsContentDetails_value(playlistItemsContentDetails *c){

    if(!strcmp(c->videoId, "") && c->videoPublishedAt.tm_year == 70){
        return true;
    }

    return false;
}


void test_getPlaylistItems_firstPage(){

     if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    WiFiClientSecure dummyClient;
    YoutubeApi dummyApi(API_KEY, dummyClient);

    YoutubePlaylist uut(&dummyApi, playlistId);

    dummyClient.setInsecure(); 

    bool ret = uut.getPlaylistItemsPage(0);

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to be able to get first PlaylistItemsPage");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkItemsConfigSet(), "Expected the configuration flag to be set!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkItemsContentDetsSet(), "Expected the data flag to be set!");

    TEST_ASSERT_MESSAGE(uut.playlistItemsConfig->currentPage == 0, "Expected to be on first page");

    playlistItemsConfiguration *uutConfig = uut.playlistItemsConfig;
    

    if(uutConfig->totalResults < YT_PLAYLIST_ITEM_RESULTS_PER_PAGE){
        TEST_ASSERT_EQUAL_STRING_MESSAGE("", uutConfig->nextPageToken, "Did not expect a next page token to be set, when results fit into one page!");
        TEST_ASSERT_EQUAL_STRING_MESSAGE("", uutConfig->previousPageToken, "Did not expect a previous page token to be set, when feetching first page!");

        TEST_ASSERT_EQUAL_UINT8_MESSAGE(uutConfig->totalResults - 1, uutConfig->currentPageLastValidPos, "Expected amount of total results correlating with lastValidPage position!");

    }else{ // full page 

        if(uutConfig->totalResults == YT_PLAYLIST_ITEM_RESULTS_PER_PAGE){
            TEST_ASSERT_MESSAGE(strcmp(uutConfig->nextPageToken, "") == 0, "Expected no next page token, as all results fit into one page!"); // Couldnt test this case.
        }else{
            TEST_ASSERT_MESSAGE(strcmp(uutConfig->nextPageToken, "") != 0, "Expected a next page token to be set!");
        }

        TEST_ASSERT_EQUAL_UINT8_MESSAGE(YT_PLAYLIST_ITEM_RESULTS_PER_PAGE - 1, uutConfig->currentPageLastValidPos, "Expected page to be filed!");
    }

    // Testing if default values are set 
   for(int i = 0; i < YT_PLAYLIST_ITEM_RESULTS_PER_PAGE; i++){

            if(i > uutConfig->currentPageLastValidPos){
                ret = checkForDefaultPlaylistItemsContentDetails_value(&uut.itemsContentDets[i]);
                TEST_ASSERT_TRUE_MESSAGE(ret, "Expected a default value!");
            }else{
                ret = checkForDefaultPlaylistItemsContentDetails_value(&uut.itemsContentDets[i]);
                TEST_ASSERT_EQUAL_MESSAGE(strlen(uut.itemsContentDets[i].videoId), YT_VIDEOID_LEN, "Expected other length of videoId string!");
                TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(104, uut.itemsContentDets[i].videoPublishedAt.tm_year, "Video upload date should be after 2004");
                TEST_ASSERT_FALSE_MESSAGE(ret, "Did not expect filler values on full page!");
            }
    }
}


bool establishInternetConnection(){
    WiFi.mode(WIFI_STA);
	WiFi.disconnect();
    delay(100);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    for(int tryWIFI = 1; tryWIFI < MAX_WIFI_RETRIES; tryWIFI++){
        if(WiFi.status() == WL_CONNECTED){
            return true;
        }
        delay(1000);
    }
    return false;
}

void setup(){

    Serial.begin(115200);

    UNITY_BEGIN();

    RUN_TEST(test_constructDestruct_simple);
    establishInternetConnection();
    RUN_TEST(test_getPlaylistStatus_simple);
    RUN_TEST(test_getPlaylistContentDetails_simple);
    RUN_TEST(test_getPlaylistSnippet_simple);

    strcpy(playlistId, TEST_PLAYLIST_ID_FEW_UPLOADS);
    RUN_TEST(test_getPlaylistItems_firstPage);

    Serial.println("Testing now with an playlistId, with more than YT_PLAYLIST_ITEM_RESULTS_PER_PAGE items");

    strcpy(playlistId, TEST_PLAYLIST_ID_MANY_UPLOADS );
    RUN_TEST(test_getPlaylistItems_firstPage);

    UNITY_END();
}

void loop(){}