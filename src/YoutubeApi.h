/*
 * YoutubeApi - An Arduino wrapper for the YouTube API
 * Copyright (c) 2020 Brian Lough
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef YoutubeApi_h
#define YoutubeApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>
#include <ctime>

#define YTAPI_HOST "www.googleapis.com"
#define YTAPI_SSL_PORT 443
#define YTAPI_TIMEOUT 1500

#define YTAPI_CHANNEL_ENDPOINT "/youtube/v3/channels"
#define YTAPI_VIDEO_ENDPOINT "/youtube/v3/videos"

#define YT_VIDEO_TITLE_MAX_LENGTH 100

struct channelStatistics {
	long viewCount;
	long commentCount;  /* DEPRECATED */
	long subscriberCount;
	bool hiddenSubscriberCount;
	long videoCount;
};


struct contentDetails{
	tm duration;
	char dimension[3];
	char defintion[3];
	bool caption;
	bool licensedContent;
};


struct videoStatistics {
	long viewCount;
	long commentCount;
	long likeCount;
//	long dislikeCount;
//	In Memory of the old dislike count.	
};


struct snippet{
	bool set;
	tm publishedAt;
	char *channelId;
	char *title;
	char *description;
	char *channelTitle;
	int categoryId;
	char *liveBroadcastContent;
	char *defaultLanguage;
	char *defaultAudioLanguage;
};

class YoutubeApi
{
	public:
		YoutubeApi(const char *key, Client &client);
		YoutubeApi(const String& apiKey, Client& client);
		int sendGetToYoutube(const char *command);
		int sendGetToYoutube(const String& command);
		bool getChannelStatistics(const char *channelId);
		bool getChannelStatistics(const String& channelId);
		bool getVideoStatistics(const char *videoId);
		bool getVideoStatistics(const String& videoId);
		bool getContentDetails(const char *videoId);
		bool getContentDetails(const String& videoId);
		bool getSnippet(const char *videoId);
		bool getSnippet(const String& videoId);
		snippet snip;
		channelStatistics channelStats;
		videoStatistics videoStats;
		contentDetails contentDets;
		bool _debug = false;

	private:
		const String apiKey;
		Client &client;
		tm parseDuration(const char *duration);
		tm parseUploadDate(const char *dateTime);
		void freeSnippet(snippet *s);
		int allocAndCopy(char **pos, const char *data);
		int getHttpStatusCode();
		void skipHeaders();
		void closeClient();	
};

#endif
