#include <unity.h>
#include <Arduino.h>
#include "YoutubeVideo.h"
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include "secrets.h" // API key and wifi password are defined in here

#define UNIT_TESTING 1
#define MAX_WIFI_RETRIES 10

const char *validIdChar = "12345678901";
const char *invalidIdChar = "123";
String validIdString = "12345678901";
String invalidIdString = "123";

void setUp(){
         if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }
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


void test_resetInfo_keepYoutubeApi_obj(){
    WiFiClientSecure client;
    YoutubeApi apiObject(API_KEY, client);
    YoutubeApi *pointerToObj = &apiObject;
    YoutubeVideo uut(validIdChar, pointerToObj);

    uut.resetInfo();

    TEST_ASSERT_EQUAL_MESSAGE(pointerToObj, uut.getYoutubeApiObj(), "YoutubeApi object should remain the same!");
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


void test_getVideoStats_simple(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoStatistics();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.videoStats, "There should be a videoStatistics object set!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoStatisticsSet(), "Video statistics flag should be set!");
}

void test_getVideoStats_simple_reset(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoStatistics();

    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.videoStats, "There should be a videoStatistics object set!");
    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkVideoStatisticsSet(), "Video statistics flag should not be set!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoStats, "Videostats should have been reset!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Expected a empty string, as video id has not been set yet!");
}


void test_getVideoSnippet_simple(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoSnippet();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoSnippetSet(), "Video snippet flag should be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.videoSnip, "There should be a snippet object set!");
}

void test_getVideoSnippet_simple_reset(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoSnippet();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoSnippetSet(), "Video snippet flag should be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.videoSnip, "There should be a snippet object set!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkVideoSnippetSet(), "Video snippet flag should not be set!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoSnip, "Videosnippet should have been reset!");
}


void test_getVideoStatus_simple(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoStatus();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoStatusSet(), "Video status flag should be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.vStatus, "There should be a videoStatus object set!");
}

void test_getVideoStatus_simple_reset(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoStatus();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoStatusSet(), "Video status flag should be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.vStatus, "There should be a videoStatus object set!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkVideoStatusSet(), "Video status flag should not be set!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.vStatus, "There should not be a videoStatus object set!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Expected a empty string, as video id has not been set yet!");
}


void test_getVideoContentDetails_simple(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoContentDetails();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoContentDetailsSet(), "Video content details flag should be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.videoContentDets, "There should be a content details object set!");
}

void test_getVideoContentDetails_simple_reset(){

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    client.setInsecure();

    YoutubeVideo uut("USKD3vPD6ZA", &apiObj);
    bool ret = uut.getVideoContentDetails();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Should be able to fetch video info!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkVideoContentDetailsSet(), "Video content details flag should be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.videoContentDets, "There should be a content details object set!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkVideoContentDetailsSet(), "Video content details flag should not be set!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.videoContentDets, "There should not be a content details object set!");
    TEST_ASSERT_EQUAL_MESSAGE(false, uut.checkVideoIdSet(), "videoId should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getVideoId(), "Expected a empty string, as video id has not been set yet!");
}

void setup()
{

    Serial.begin(115200);

    UNITY_BEGIN();

    establishInternetConnection();

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
    RUN_TEST(test_resetInfo_keepYoutubeApi_obj);


    RUN_TEST(test_getVideoStats_simple);
    delay(100);
    RUN_TEST(test_getVideoStats_simple_reset);
    delay(100);
    RUN_TEST(test_getVideoSnippet_simple);
    delay(100);
    RUN_TEST(test_getVideoSnippet_simple_reset);
    delay(100);
    RUN_TEST(test_getVideoStatus_simple);
    delay(100);
    RUN_TEST(test_getVideoStatus_simple_reset);
    delay(100);
    RUN_TEST(test_getVideoContentDetails_simple);
    delay(100);
    RUN_TEST(test_getVideoContentDetails_simple_reset);

    UNITY_END();
}

void loop()
{
}
