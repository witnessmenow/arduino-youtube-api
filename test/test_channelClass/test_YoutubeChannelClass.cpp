#include <Arduino.h>
#include <unity.h>
#include "YoutubeChannel.h"
#include "secrets.h"
#include <WiFiClientSecure.h>

#define MAX_WIFI_RETRIES 10

#define validChannelId "123456789012345678901234"
#define tooLongChannelId "1234567890123456789012345"
#define tooShortChannelId "12345678901234567890123"


void test_onlyApiConstructor(){
    WiFiClientSecure client;
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(&apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "Channel id should be empty");
}

void test_charConstructor_validId(){
    WiFiClientSecure client;
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_CHANNEL_ID, uut.getChannelId(), "Channel id wrong!");
}

void test_charConstructor_tooLongId(){
    WiFiClientSecure client;
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(tooLongChannelId, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "Channel id should be empty");
}

void test_charConstructor_tooShortId(){
    WiFiClientSecure client;
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(tooShortChannelId, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "Channel id should be empty");
}

void test_resetInfo(){
    WiFiClientSecure client;
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(validChannelId, &apiDummy);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(validChannelId, uut.getChannelId(), "Channel id not correct!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "channel id should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "api object pointer should stay");
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

void test_getChannelStatistics_simple(){

    if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiObj);

    client.setInsecure();

    bool ret = uut.getChannelStatistics();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to receive valid response!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelStatsSet(), "Expected the channel statistics flag to be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelStats, "Expected a channelStatistics object to be set!");
}

void test_getChannelStatistics_simple_reset(){

    if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    WiFiClientSecure client;
    YoutubeApi apiObj(API_KEY, client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiObj);

    client.setInsecure();

    bool ret = uut.getChannelStatistics();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to receive valid response!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelStatsSet(), "Expected the channel statistics flag to be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelStats, "Expected a channelStatistics object to be set!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelStatsSet(), "Expected the channel statistics flag to not be set!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.channelStats, "Expected the channelStatistics object to not be set!");

}



void setup(){
    Serial.begin(115200);
    UNITY_BEGIN();

    RUN_TEST(test_onlyApiConstructor);

    RUN_TEST(test_charConstructor_validId);
    RUN_TEST(test_charConstructor_tooShortId);
    RUN_TEST(test_charConstructor_tooLongId);

    RUN_TEST(test_resetInfo);

    establishInternetConnection();

    RUN_TEST(test_getChannelStatistics_simple);
    RUN_TEST(test_getChannelStatistics_simple_reset);

    UNITY_END();
}

void loop(){}
