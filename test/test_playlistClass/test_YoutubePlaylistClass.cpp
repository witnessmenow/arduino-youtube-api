#include <unity.h>
#include <Arduino.h>
#include "secrets.h"
#include "YoutubePlaylist.h"
#include "YoutubeTypes.h"
#include <WiFiClientSecure.h>


#define MAX_WIFI_RETRIES 10

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

    UNITY_END();
}

void loop(){}