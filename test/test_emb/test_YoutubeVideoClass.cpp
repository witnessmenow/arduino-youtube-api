#include <unity.h>
#include <Arduino.h>
#include "YoutubeVideo.h"

#define UNIT_TESTING 1

void setUp(){

}

void tearDown(){

}

void test_emptyConstructor(){
    YoutubeVideo uut;

    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoSnip, "videoSnip pointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoStats, "videoStats pointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoContentDets, "videoContentDets pointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.vStatus, "vStatus pPointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
    
}


void test_constCharConstructor_simple(){
    YoutubeVideo uut("epic");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("epic", uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_StringConstructor_simple(){
    String testString = "epic";
    YoutubeVideo uut(testString);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("epic", uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}


void test_resetVideoId_simple(){
    YoutubeVideo uut("epic");

    bool ret = uut.resetVideoId("oof");
    TEST_ASSERT_EQUAL(true, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("oof", uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}


void setup(){

    Serial.begin(115200);

    UNITY_BEGIN();

    RUN_TEST(test_emptyConstructor);
    RUN_TEST(test_constCharConstructor_simple);
    RUN_TEST(test_StringConstructor_simple);
    RUN_TEST(test_resetVideoId_simple);

    UNITY_END();
}

void loop(){

}