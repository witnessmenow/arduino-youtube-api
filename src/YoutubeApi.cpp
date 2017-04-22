/*
   Copyright (c) 2017 Brian Lough. All right reserved.

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


#include "YoutubeApi.h"

YoutubeApi::YoutubeApi(String clientId, String clientSecret, String refreshToken, Client &client) {
	_clientId = clientId;
	_clientSecret = clientSecret;
	oAuth2Token.refreshToken = refreshToken;
	oAuth2Token.expiresIn = 0;
	oAuth2Token.lastRefreshedAt = 0;
	this->client = &client;
}

String YoutubeApi::sendGetToYoutube(String command) {
	String headers = "";
	String body = "";
	bool finishedHeaders = false;
	bool currentLineIsBlank = true;
	unsigned long now;
	bool avail;

	if(getAccessToken()) {
		// Connect with youtube api over ssl
		if(client->connect(YTAPI_HOST, YTAPI_SSL_PORT)) {
			String a = "";
			char c;
			int ch_count = 0;
			command = "https://" YTAPI_HOST + command + "&access_token=" + oAuth2Token.accessToken;
			client->println("GET " + command);
			now = millis();
			avail = false;
			while(millis() - now < YTAPI_TIMEOUT) {
				while(client->available()) {
					// Allow body to be parsed before finishing
					avail = finishedHeaders;
					char c = client->read();

					if(!finishedHeaders) {
						if(currentLineIsBlank && c == '\n') {
							finishedHeaders = true;
						}
						else {
							headers = headers + c;
						}
					} else {
						if(ch_count < maxMessageLength)  {
							body = body+c;
							ch_count++;
						}
					}

					if(c == '\n') {
						currentLineIsBlank = true;
					} else if(c != '\r') {
						currentLineIsBlank = false;
					}
				}
				if(avail) {
					// Serial.println("Body:");
					// Serial.println(body);
					// Serial.println("END");
					break;
				}
			}
		}
		return body;
	}
	return "Failed to retrieve valid access token.";
}

/**
 * This method sends a HTTP POST to the YouTube API host and is used for
 * instance to refresh the OAuth2 access token.
 *
 * Reference:
 * http://playground.arduino.cc/Code/WebClient
 **/
String YoutubeApi::sendPostToYouTube(String page, String postData) {
  if(client->connect(YTAPI_HOST, YTAPI_SSL_PORT)) {
    client->println("POST " + page + " HTTP/1.1");
    client->println("Host: " YTAPI_HOST);
    client->println("Content-Type: application/x-www-form-urlencoded");
    client->println("Connection: close");
    client->print("Content-Length: ");
    client->println(postData.length());
    client->println();
    client->println(postData);
  }

  String headers = "";
	String body = "";
	bool finishedHeaders = false;
	bool currentLineIsBlank = true;
	unsigned long now;
	bool avail;
  String a = "";
  char c;
  int ch_count = 0;
  now = millis();
  avail = false;

  while(millis() - now < YTAPI_TIMEOUT) {
    while(client->available()) {
      // Allow body to be parsed before finishing
      avail = finishedHeaders;
      char c = client->read();

      if(!finishedHeaders) {
        if(currentLineIsBlank && c == '\n') {
          finishedHeaders = true;
        }
        else {
          headers = headers + c;
        }
      } else {
        if(ch_count < maxMessageLength)  {
          body = body+c;
          ch_count++;
        }
      }
      if(c == '\n') {
        currentLineIsBlank = true;
      } else if(c != '\r') {
        currentLineIsBlank = false;
      }
    }
    if(avail) {
      // Serial.println("Body:");
      // Serial.println(body);
      // Serial.println("END");
      break;
    }
  }
	return body;
}

/**
 * https://developers.google.com/youtube/v3/docs/channels/list
 */
bool YoutubeApi::getChannelStatistics(String channelId) {
	String command = "/youtube/v3/channels?part=statistics&id=" + channelId; //If you can't find it(for example if you have a custom url) look here: https://www.youtube.com/account_advanced
	String response = sendGetToYoutube(command); //receive reply from youtube
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
	if(root.success()) {
		if (root.containsKey("items")) {
			long subscriberCount = root["items"][0]["statistics"]["subscriberCount"];
			long viewCount = root["items"][0]["statistics"]["viewCount"];
			long commentCount = root["items"][0]["statistics"]["commentCount"];
			long hiddenSubscriberCount = root["items"][0]["statistics"]["hiddenSubscriberCount"];
			long videoCount = root["items"][0]["statistics"]["videoCount"];

			channelStats.viewCount = viewCount;
			channelStats.subscriberCount = subscriberCount;
			channelStats.commentCount = commentCount;
			channelStats.hiddenSubscriberCount = hiddenSubscriberCount;
			channelStats.videoCount = videoCount;

			return true;
		}
	}
	return false;
}

/**
 * This method returns the 3 most recent subscribers to the channel.
 *
 * TODO This method should eventually be implemented as getRecentSubscribers()
 * with all the available YouTube API parameters.
 *
 * The pageToken is the String for the next (or previous) set of subscribers.
 *
 * Return the next page string token if succesfull otherwise the error
 * message that was returned by the YouTube API
 *
 * References
 * https://www.googleapis.com/youtube/v3/subscriptions
 * https://developers.google.com/youtube/v3/docs/subscriptions/list
 */
String YoutubeApi::getMyRecentSubscribers(String pageToken) {
	String command = "/youtube/v3/subscriptions?part=subscriberSnippet&myRecentSubscribers=true&maxResults=3&pageToken=" + pageToken;
	String response = sendGetToYoutube(command);
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
	if(root.success()) {
		if (root.containsKey("items")) {
			for (int i = 0; i < root["items"].size(); i++) {
				String subscriber = root["items"][i]["subscriberSnippet"]["title"];
				myRecentSubscribers[i] = subscriber;
			}
			return root["nextPageToken"];
		}
	}
	String code = root["error"]["code"];
	String message = root["error"]["message"];
	return "error," + code + "," + message;
}

String YoutubeApi::getMyRecentSubscribers() {
	return getMyRecentSubscribers("");
}

/**
 * The access token is only valid for 3600 seconds (by default) before it needs
 * to be refreshed.
 */
bool YoutubeApi::getAccessToken() {
	unsigned long currentTime = millis();
	unsigned long tokenExpiresAt = oAuth2Token.lastRefreshedAt + (oAuth2Token.expiresIn * 1000);

	String page = "/oauth2/v4/token";		// https://developers.google.com/youtube/v3/guides/auth/devices
	String myClientId(_clientId);	// TODO Is there a better way to turn char[] into a String?
	String myClientSecret(_clientSecret);
	String postData = "client_id=" + myClientId + "&client_secret=" + myClientSecret + "&refresh_token=" + oAuth2Token.refreshToken + "&grant_type=refresh_token";

	// Only get a new access token if the old one is expired
	if(currentTime > tokenExpiresAt) {
		String response = sendPostToYouTube(page, postData);
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response);
		if(root.success()) {
			if(!root.containsKey("error")) {
				String accessToken = root["access_token"];
				String tokenType = root["token_type"];
				int expiresIn = root["expires_in"];

				oAuth2Token.accessToken = accessToken;
				oAuth2Token.tokenType = tokenType;
				oAuth2Token.expiresIn = expiresIn;
				oAuth2Token.lastRefreshedAt = millis();

				return true;
			}
			root.printTo(Serial);
		}
		return false;
	}
	// Serial.println("Don't need to refresh the token. Current access token is still valid!");
	return true;
}
