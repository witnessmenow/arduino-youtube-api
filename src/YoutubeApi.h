/*
Copyright (c) 2020 Brian Lough. All right reserved.

YoutubeApi - An Arduino wrapper for the YouTube API

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

#define YTAPI_HOST "www.googleapis.com"
#define YTAPI_SSL_PORT 443
#define YTAPI_TIMEOUT 1500

#define YTAPI_CHANNEL_ENDPOINT "/youtube/v3/channels"

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
    YoutubeApi (char *apiKey, Client &client);
    YoutubeApi (String apiKey, Client &client);
    int sendGetToYoutube(char *command);
    bool getChannelStatistics(char *channelId);
    bool getChannelStatistics(String channelId);
    channelStatistics channelStats;
    bool _debug = false;

  private:
    char *_apiKey;
    Client *client;
    int getHttpStatusCode();
    void skipHeaders();
    void closeClient();
};

#endif
