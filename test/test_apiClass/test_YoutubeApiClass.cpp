#include <unity.h>
#include <Arduino.h>
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


void setup()
{

    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(test_createRequestString_videoStats_simple);
    RUN_TEST(test_createRequestString_videoStats_length40);
    RUN_TEST(test_createRequestString_videoStats_length46);
    UNITY_END();
}

void loop()
{
}