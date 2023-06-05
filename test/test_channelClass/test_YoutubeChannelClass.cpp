#include <Arduino.h>
#include <unity.h>
#include "YoutubeChannel.h"
#include "secrets.h"
#include <WiFiClientSecure.h>

#define MAX_WIFI_RETRIES 10

#define tooLongChannelId "1234567890123456789012345"
#define tooShortChannelId "12345678901234567890123"

WiFiClientSecure client;

void setUp(){

    if(WiFi.status() != WL_CONNECTED){
        TEST_IGNORE_MESSAGE("Could not establish internet connection!");
    }

    client = WiFiClientSecure();
    client.setInsecure();
}

void test_onlyApiConstructor(){
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(&apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "Channel id should be empty");
}

void test_charConstructor_validId(){
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_CHANNEL_ID, uut.getChannelId(), "Channel id wrong!");
}

void test_charConstructor_tooLongId(){
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(tooLongChannelId, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "Channel id should be empty");
}

void test_charConstructor_tooShortId(){
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(tooShortChannelId, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should not be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", uut.getChannelId(), "Channel id should be empty");
}

void test_resetInfo(){
    YoutubeApi apiDummy("", client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiDummy);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_CHANNEL_ID, uut.getChannelId(), "Channel id not correct!");

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
    YoutubeApi apiObj(API_KEY, client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiObj);

    bool ret = uut.getChannelStatistics();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to receive valid response!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelStatsSet(), "Expected the channel statistics flag to be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelStats, "Expected a channelStatistics object to be set!");
}

void test_getChannelStatistics_simple_reset(){
    YoutubeApi apiObj(API_KEY, client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiObj);

    bool ret = uut.getChannelStatistics();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to receive valid response!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelStatsSet(), "Expected the channel statistics flag to be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelStats, "Expected a channelStatistics object to be set!");

    uut.resetInfo();

    TEST_ASSERT_FALSE_MESSAGE(uut.checkChannelStatsSet(), "Expected the channel statistics flag to not be set!");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, uut.channelStats, "Expected the channelStatistics object to not be set!");

}

void test_getChannelSnippet_simple(){
    YoutubeApi apiObj(API_KEY, client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiObj);

    bool ret = uut.getChannelSnippet();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to receive valid response!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelSnipSet(), "Expected the channel statistics flag to be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelSnip, "Expected a channelSnip to be set!");
}

void test_getChannelContentDetails_simple(){
    YoutubeApi apiObj(API_KEY, client);
    YoutubeChannel uut(TEST_CHANNEL_ID, &apiObj);
    
    bool ret = uut.getChannelContentDetails();

    TEST_ASSERT_TRUE_MESSAGE(ret, "Expected to receive valid response!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelContentDetailsSet(), "Expected the channel contentDetails flag to be set!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelContentDets, "Expected a contentDetails to be set!");

    // checking for valid strings (not NULL)
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelContentDets->relatedPlaylistsLikes, "Expected a valid string");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(NULL, uut.channelContentDets->relatedPlaylistsUploads, "Expected a valid string");
}



void setup(){
    Serial.begin(115200);
    UNITY_BEGIN();

    establishInternetConnection();

    RUN_TEST(test_onlyApiConstructor);

    RUN_TEST(test_charConstructor_validId);
    RUN_TEST(test_charConstructor_tooShortId);
    RUN_TEST(test_charConstructor_tooLongId);

    RUN_TEST(test_resetInfo);

    RUN_TEST(test_getChannelStatistics_simple);
    RUN_TEST(test_getChannelStatistics_simple_reset);

    RUN_TEST(test_getChannelSnippet_simple);

    RUN_TEST(test_getChannelContentDetails_simple);

    UNITY_END();
}

void loop(){}
