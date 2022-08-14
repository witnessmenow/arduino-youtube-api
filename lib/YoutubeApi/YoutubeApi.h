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
#include "YoutubeTypes.h"
#include <ArduinoJson.h>
#include <Client.h>

class YoutubeApi
{
	public:
		YoutubeApi(const char *key, Client &newClient);
		YoutubeApi(const String& key, Client& newClient);

		static bool createRequestString(int mode, char *command, const char *id);

		int sendGetToYoutube(const char *command);
		int sendGetToYoutube(const String& command);

		bool getChannelStatistics(const String& channelId);
		bool getChannelStatistics(const char *channelId);

		bool getVideoContentDetails(const String& videoId);
		bool getVideoContentDetails(const char *videoId);

		bool getVideoStatus(const String& videoId);
		bool getVideoStatus(const char *videoId);

		static int allocAndCopy(char **pos, const char *data);
		static tm parseUploadDate(const char *dateTime);

		channelStatistics channelStats;

		videoSnippet videoSnip;
		videoStatistics videoStats;
		videoContentDetails videoContentDets;
		videoStatus vStatus;
		bool _debug = false;
		Client &client;

		void closeClient();	

	private:
		static char apiKey[YTAPI_KEY_LEN + 1];
		// Client &client;
		tm parseDuration(const char *duration);

		void freeVideoSnippet(videoSnippet *s);
		void freeVideoStatus(videoStatus *s);
		bool getRequestedType(int op, const char *channelId);
		int getHttpStatusCode();

		bool parseChannelStatistics();
		bool parseVideoContentDetails();
		bool parseVideoStatus();

		void skipHeaders();
};

#endif
