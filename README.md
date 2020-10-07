# Arduino YouTube API Library
[![arduino-library-badge](https://www.ardu-badge.com/badge/YoutubeApi.svg?)](https://www.ardu-badge.com/YoutubeApi) [![Build Status](https://travis-ci.org/witnessmenow/arduino-youtube-api.svg?branch=master)](https://travis-ci.org/witnessmenow/arduino-youtube-api)

A wrapper for the [YouTube API](https://developers.google.com/youtube/v3/docs/) for Arduino. Works on both ESP8266 and ESP32.

![Imgur](http://i.imgur.com/FmXyW4E.png)

### Support what I do!

I have created a lot of different Arduino libraries that I hope people can make use of. [If you enjoy my work, please consider becoming a GitHub sponsor!](https://github.com/sponsors/witnessmenow/)

## Getting a Google Apps API key (Required!)

* Go to [the Google developer dashboard](https://console.developers.google.com) and create a new project
* In the "API & Services" menu, [go to "Credentials"](https://console.developers.google.com/apis/credentials) and create a new API key.
* Head to [the Google API library](https://console.developers.google.com/apis/library), find the ["YouTube Data API v3"](https://console.developers.google.com/apis/library/youtube.googleapis.com), and "Enable" it for your project.
* Make sure the following URL works for you in your browser (change the key at the end!):

`https://www.googleapis.com/youtube/v3/channels?part=statistics&id=UCezJOfu7OtqGzd5xrP3q6WA&key=PutYourNewlyGeneratedKeyHere`

If everything is working correctly you should see the JSON channel statistics for [my YouTube channel](https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA)!

## Installing

The easiest way to install this library is through [the Arduino Library Manager](https://www.arduino.cc/en/Guide/Libraries), just search for "Youtube". Alternately, you can also download [the latest version](../../releases/latest) and import it into the IDE with the "Add .ZIP Library" feature.

This library depends on [the ArduinoJson library](https://github.com/bblanchon/ArduinoJson) by [Benoît Blanchon](https://github.com/bblanchon) for parsing messages from the API. If you are using Arduino 1.8.10 or newer the library manager will install this automatically alongside YoutubeApi. If you are using an older version of Arduino you will have to download it yourself: search for "ArduinoJson" in the Arduino Library Manager or download the latest release from [here](https://github.com/bblanchon/ArduinoJson/releases/latest).

## Basic Use

Include YoutubeApi in your project:

    #include <YoutubeApi.h>

and pass it an API key and an SSL client:

    #define API_KEY "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    WiFiClientSecure client; //For ESP8266 and ESP32 boards
    YoutubeApi api(API_KEY, client);

then once you're connected to WiFi you can start requesting data from the API:

    #define CHANNEL_ID "UCezJOfu7OtqGzd5xrP3q6WA"

    if(api.getChannelStatistics(CHANNEL_ID)) {
        Serial.print("Subscriber Count: ");
        Serial.println(api.channelStats.subscriberCount);
    }

If you don't know it, you can find your own YouTube channel ID [here](https://www.youtube.com/account_advanced). See [the examples](examples) for more details on how to use the library.

## Supported Data Methods

Currently the only implemented method is getting channel statistics, but the library can be easily extended. Please raise an issue if there is a method you are looking for.

## License

This library is licensed under the terms of [the MIT License](LICENSE.md).
