#include <Arduino.h>
#include <unity.h>
#include "YoutubeChannel.h"
#include <WiFiClientSecure.h>

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
    YoutubeChannel uut(validChannelId, &apiDummy);

    TEST_ASSERT_EQUAL_MESSAGE(&apiDummy, uut.getYoututbeApiObj(), "Expected vaild apiObject!");
    TEST_ASSERT_TRUE_MESSAGE(uut.checkChannelIdSet(), "Channel id flag should be set");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(validChannelId, uut.getChannelId(), "Channel id should be empty");
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


void setup(){
    Serial.begin(115200);
    UNITY_BEGIN();

    RUN_TEST(test_onlyApiConstructor);

    RUN_TEST(test_charConstructor_validId);
    RUN_TEST(test_charConstructor_tooShortId);
    RUN_TEST(test_charConstructor_tooLongId);

    RUN_TEST(test_resetInfo);

    UNITY_END();
}

void loop(){}