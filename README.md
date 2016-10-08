# arduino-youtube-api
A wrapper for the YouTube API for Arduino (works on ESP8266)

Currently the only implemented method is getting the channel statistics but it can be easily extended.

![Imgur](http://i.imgur.com/FmXyW4E.png)

## Getting a Google Apps API key (Required!)

* Create an application [here](https://console.developers.google.com)
* On the API Manager section, go to "Credentials" and create a new API key
* Enable your application to communicate the YouTube Api [here](https://console.developers.google.com/apis/api/youtube)
* Make sure the following URL works for you in your browser (Change the key at the end!):
https://www.googleapis.com/youtube/v3/channels?part=statistics&id=UCu7_D0o48KbfhpEohoP7YSQ&key=PutYourNewlyGeneratedKeyHere
