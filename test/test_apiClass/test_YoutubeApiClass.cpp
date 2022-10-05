#include <unity.h>
#include <Arduino.h>
#include "secrets.h"
#include "YoutubeApi.h"
#include "YoutubeTypes.h"
#include <WiFiClientSecure.h>


void test_createRequestString_videoStats_simple(){
    WiFiClientSecure client;
    YoutubeApi uut("123", client);

    char uutCommand[150];
    const char *expectedRes = "/youtube/v3/videos?part=statistics&id=456&key=123";
    YoutubeApi::createRequestString(videoListStats, uutCommand, "456");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}

void test_createRequestString_videoStats_length40(){
    WiFiClientSecure client;
    char Length40ApiKey[41] = "1234567890123456789012345678901234567890";
    YoutubeApi uut(Length40ApiKey, client);

    char uutCommand[150];
    const char *expectedRes = "/youtube/v3/videos?part=statistics&id=456&key=1234567890123456789012345678901234567890";
    YoutubeApi::createRequestString(videoListStats, uutCommand, "456");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}

void test_createRequestString_videoStats_length46(){
    WiFiClientSecure client;
    char length41ApiKey[47] = "1234567890123456789012345678901234567890123456";
    YoutubeApi uut(length41ApiKey, client);

    char uutCommand[150];
    const char *expectedRes = "/youtube/v3/videos?part=statistics&id=456&key=123456789012345678901234567890123456789012345"; // should be cut off
    YoutubeApi::createRequestString(videoListStats, uutCommand, "456");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}

void test_createRequestString_videoSnip_length40(){
    WiFiClientSecure client;
    YoutubeApi uut(API_KEY, client);

    char uutCommand[150];
    char expectedRes[150];
    sprintf(expectedRes, "/youtube/v3/videos?part=snippet&id=%s&key=%s", TEST_VID_ID, API_KEY);
    YoutubeApi::createRequestString(videoListSnippet, uutCommand, TEST_VID_ID);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}

void test_createRequestString_channelStatistics_simple(){
    WiFiClientSecure client;
    YoutubeApi uut(API_KEY, client);

    char uutCommand[150];
    char expectedRes[150];
    sprintf(expectedRes, "/youtube/v3/channels?part=statistics&id=%s&key=%s", TEST_CHANNEL_ID, API_KEY);
    YoutubeApi::createRequestString(channelListStats, uutCommand, TEST_CHANNEL_ID);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}

void test_createRequestString_channelSnippet_simple(){
    WiFiClientSecure client;
    YoutubeApi uut(API_KEY, client);

    char uutCommand[150];
    char expectedRes[150];
    sprintf(expectedRes, "/youtube/v3/channels?part=snippet&id=%s&key=%s", TEST_CHANNEL_ID, API_KEY);
    YoutubeApi::createRequestString(channelListSnippet, uutCommand, TEST_CHANNEL_ID);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}

void test_createRequestString_channelContentDetails_simple(){
    WiFiClientSecure client;
    YoutubeApi uut(API_KEY, client);

    char uutCommand[150];
    char expectedRes[150];
    sprintf(expectedRes, "/youtube/v3/channels?part=contentDetails&id=%s&key=%s", TEST_CHANNEL_ID, API_KEY);
    YoutubeApi::createRequestString(channelListContentDetails, uutCommand, TEST_CHANNEL_ID);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}


void test_allocAndCopy_pos_NULL(){

    const char someData[] = "testdata";

    int ret = YoutubeApi::allocAndCopy(NULL, someData);

    TEST_ASSERT_EQUAL_MESSAGE(1, ret, "There should be an error copying to NULL :p!");

}

void test_allocAndCopy_data_NULL(){

    char *destinationOfCopy;

    int ret = YoutubeApi::allocAndCopy(&destinationOfCopy, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(1, ret, "There should be an error copying from NULL :p!");

}

void test_allocAndCopy_simple(){

    char *destinationOfCopy;
    const char someData[] = "testdata";

    int ret = YoutubeApi::allocAndCopy(&destinationOfCopy, someData);

    TEST_ASSERT_EQUAL_MESSAGE(0, ret, "There should not be an error!");
    if(destinationOfCopy == NULL){
        TEST_FAIL_MESSAGE("Data destination has become a NULL pointer!");
    }
    TEST_ASSERT_EQUAL_STRING_MESSAGE(someData, destinationOfCopy, "Data should match!");

    free(destinationOfCopy);
}


void setup()
{

    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(test_createRequestString_videoStats_simple);
    RUN_TEST(test_createRequestString_videoStats_length40);
    RUN_TEST(test_createRequestString_videoStats_length46);
    RUN_TEST(test_createRequestString_videoSnip_length40);

    RUN_TEST(test_createRequestString_channelStatistics_simple);
    RUN_TEST(test_createRequestString_channelSnippet_simple);
    RUN_TEST(test_createRequestString_channelContentDetails_simple);

    RUN_TEST(test_allocAndCopy_pos_NULL);
    RUN_TEST(test_allocAndCopy_data_NULL);
    RUN_TEST(test_allocAndCopy_simple);
    
    UNITY_END();
}

void loop()
{
}