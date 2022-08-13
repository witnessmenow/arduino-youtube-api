#include <unity.h>
#include <Arduino.h>
#include "YoutubeVideo.h"

#define UNIT_TESTING 1

const char *validIdChar = "12345678901";
const char *invalidIdChar = "123";
String validIdString = "12345678901";
String invalidIdString = "123";

void test_emptyConstructor()
{
    YoutubeVideo uut;

    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoSnip, "videoSnip pointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoStats, "videoStats pointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoContentDets, "videoContentDets pointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.vStatus, "vStatus pPointer supposed to be NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
}

void test_constCharConstructor_simple()
{
    YoutubeVideo uut(validIdChar, NULL);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(validIdChar, uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_constCharConstructor_rejectId()
{
    YoutubeVideo uut(invalidIdChar, NULL);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should be set");
}

void test_StringConstructor_simple()
{
    YoutubeVideo uut(validIdString, NULL);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(validIdChar, uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_StringConstructor_rejectId()
{
    YoutubeVideo uut(invalidIdString, NULL);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should be set");
}

void test_resetVideoIdConstChar_simple()
{
    YoutubeVideo uut(validIdChar, NULL);

    bool ret = uut.resetVideoId("10987654321");
    TEST_ASSERT_EQUAL(true, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("10987654321", uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_resetVideoIdConstChar_videoId_notSet()
{
    YoutubeVideo uut;

    bool ret = uut.resetVideoId(validIdChar);
    TEST_ASSERT_EQUAL(true, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(validIdChar, uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_resetVideoIdConstChar_videoId_toLong()
{
    YoutubeVideo uut(validIdChar, NULL);

    const char videoId[13] = "012345678910";

    bool ret = uut.resetVideoId(videoId);
    TEST_ASSERT_EQUAL(false, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
}

void test_resetVideoIdConstChar_videoId_toShort()
{
    YoutubeVideo uut(validIdChar, NULL);

    const char videoId[11] = "0123456789";
    bool ret = uut.resetVideoId(videoId);
    TEST_ASSERT_EQUAL(false, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
}

void test_getVideoIdConstChar_videoId_notSet(){
    YoutubeVideo uut;

    const char* vidId = uut.getVideoId();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", vidId, "Expected a empty string, as video id has not been set yet!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
}

void test_getVideoIdConstChar_videoId_set(){
    YoutubeVideo uut(validIdChar, NULL);

    const char* vidId = uut.getVideoId();
    TEST_ASSERT_EQUAL_STRING_MESSAGE(validIdChar, vidId, "Did not return correct string");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_resetInfo_afterConstruct(){
    YoutubeVideo uut(validIdChar, NULL);
    uut.resetInfo();

    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Expected a empty string, as video id has not been set yet!");

}

void setup()
{

    Serial.begin(115200);

    UNITY_BEGIN();

    RUN_TEST(test_emptyConstructor);
    RUN_TEST(test_constCharConstructor_simple);
    RUN_TEST(test_constCharConstructor_rejectId);

    RUN_TEST(test_StringConstructor_simple);
    RUN_TEST(test_resetVideoIdConstChar_simple);
    RUN_TEST(test_StringConstructor_rejectId);

    RUN_TEST(test_resetVideoIdConstChar_videoId_notSet);
    RUN_TEST(test_resetVideoIdConstChar_videoId_toLong);
    RUN_TEST(test_resetVideoIdConstChar_videoId_toShort);

    RUN_TEST(test_getVideoIdConstChar_videoId_notSet);
    RUN_TEST(test_getVideoIdConstChar_videoId_set);

    RUN_TEST(test_resetInfo_afterConstruct);

    UNITY_END();
}

void loop()
{
}