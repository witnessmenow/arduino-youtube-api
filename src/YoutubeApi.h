/*
Copyright (c) 2015 Giancarlo Bacchio. All right reserved.

TelegramBot - Library to create your own Telegram Bot using
ESP8266 on Arduino IDE.
Ref. Library at https:github/esp8266/Arduino

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/


#ifndef YoutubeApi_h
#define YoutubeApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define HOST "www.googleapis.com"
#define SSL_PORT 443
#define HANDLE_MESSAGES 1
#define MAX_BUFFER_SIZE 1250



struct channelStatistics{
  long viewCount;
  long commentCount;
  long subscriberCount;
  bool hiddenSubscriberCount;
  long videoCount;
};

class YoutubeApi
{
  public:
    YoutubeApi (String apiKey, Client &client);
    String sendGetToYoutube(String command);
    bool getChannelStatistics(String channelId);
    channelStatistics channelStats;
    const char* fingerprint = "3F:AE:17:DA:03:65:45:BE:CE:77:2F:8A:DC:5B:C9:08:98:7E:9E:E7";  //Telegram.org Certificate

  private:
    //JsonObject * parseUpdates(String response);
    String _apiKey;
    Client *client;
    const int maxMessageLength = 1000;
    bool checkForOkResponse(String response);
};

#endif
