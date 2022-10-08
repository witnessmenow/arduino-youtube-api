#include <unity.h>
#include <Arduino.h>
#include "secrets.h"
#include "YoutubePlaylist.h"
#include "YoutubeTypes.h"
#include <WiFiClientSecure.h>


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


void setup(){

    Serial.begin(115200);

    UNITY_BEGIN();

    RUN_TEST(test_constructDestruct_simple);

    UNITY_END();
}

void loop(){}