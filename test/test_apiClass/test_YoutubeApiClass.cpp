#include <unity.h>
#include <Arduino.h>
#include "YoutubeApi.h"
#include <WiFiClientSecure.h>


void test_createRequestString_videoStats_simple(){
    WiFiClientSecure client;
    YoutubeApi uut("123", client);

    char uutCommand[150];
    const char *expectedRes = "/youtube/v3/videos?part=statistics&id=456&key=123";
    YoutubeApi::createRequestString(videoListStats, uutCommand, "456");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expectedRes, uutCommand, "The request string is not correct!");    
}


void setup()
{

    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(test_createRequestString_videoStats_simple);
    UNITY_END();
}

void loop()
{
}