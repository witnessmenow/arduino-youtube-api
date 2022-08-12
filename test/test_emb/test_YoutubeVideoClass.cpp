#include <unity.h>
#include <Arduino.h>
#include "YoutubeVideo.h"

#define UNIT_TESTING 1

const char *validIdChar = "12345678901";
const char *invalidIdChar = "123";
String validIdString = "12345678901";
String invalidIdString = "123";

void setUp()
{
}

void tearDown()
{
}

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
    YoutubeVideo uut(validIdChar);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(validIdChar, uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_constCharConstructor_rejectId()
{
    YoutubeVideo uut(invalidIdChar);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should be set");
}

void test_StringConstructor_simple()
{
    YoutubeVideo uut(validIdString);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(validIdChar, uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(true, uut.checkVideoIdSet(), "videoId should be set");
}

void test_StringConstructor_rejectId()
{
    YoutubeVideo uut(invalidIdString);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Did not return right videoId!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should be set");
}

void test_resetVideoIdConstChar_simple()
{
    YoutubeVideo uut(validIdChar);

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
    YoutubeVideo uut(validIdChar);

    const char videoId[15] = "01234567891011";

    bool ret = uut.resetVideoId(videoId);
    TEST_ASSERT_EQUAL(false, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should be set");
}

void test_resetVideoIdConstChar_videoId_toShort()
{
    YoutubeVideo uut(validIdChar);

    bool ret = uut.resetVideoId(invalidIdChar);
    TEST_ASSERT_EQUAL(false, ret);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "VideoId did not change correctly!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should be set");
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

    UNITY_END();
}

void loop()
{
}