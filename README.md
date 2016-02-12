# ESP_Web_SSD1306_epostit
ESP8266 + SSD1306 OLED display message-displayer "e-postit note"

It has an ESP8266 model ESP12 wifi microcontroller programmed with this Arduino sketch, plus an SSD1306 OLED display (I2C). It boots up, finds a WiFi network, and puts a textfile from the web on the screen every minute. (The fancy stuff to make it smart is meant to be done on the web side -- this code is fairly dumb).
