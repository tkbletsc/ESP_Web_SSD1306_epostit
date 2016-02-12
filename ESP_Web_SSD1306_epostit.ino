#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ESP8266 + SSD1306 OLED display message-displayer "e-postit note"
// By Tyler Bletsch (Tyler.Bletsch@gmail.com)

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

struct known_ssid {
  char ssid[24];
  char password[24];
};

const struct known_ssid known_ssids[] = {
  { "DUKE",         ""            },
};
const int num_known_ssids = sizeof(known_ssids)/sizeof(*known_ssids);

int WIFI_TIMEOUT = 5000;

#define my_print2(v,f)   { Serial.print(v,f);   display.print(v,f);   display.display(); }
#define my_print(v)   { Serial.print(v);   display.print(v);   display.display(); }
#define my_println(v) { Serial.println(v); display.println(v); display.display(); }
#define my_clear(v) { Serial.print("\n*CLEAR*\n\n"); display.clearDisplay(v); display.setCursor(0,0); }

int pin_button = 0; // GPIO pin for refresh button

const int UPDATE_RATE = 30000;

int time_last_update = 0;

void do_update() {
  HTTPClient http;

  my_clear();
  my_print("* ");
  http.begin("SOMEWEBSITE.COM", 80, "/PATH/TO/MESSAGE");  // < change this to where your message lives (plaintext)

  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode) {
    // HTTP header has been send and Server response header has been handled

    // file found at server
    if(httpCode == 200) {
      my_clear();
      String payload = http.getString();
      my_println(payload);
    } else {
      my_print("HTTP GET error: "); my_println(httpCode);
    }
  } else {
      my_println("HTTP GET fail!");
  }
}

void setup()   {                
  Serial.begin(9600);
  Wire.begin(14,12); //sda,scl

  pinMode(pin_button,INPUT_PULLUP);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //  ^ adafruit comment, no idea wtf they're talking about -tkb
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  my_clear();

  const char* ssid;
  const char* password;

  //5C:CF:7F:F:6E:BE
  my_print("MAC ");
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  for (int i=0; i<WL_MAC_ADDR_LENGTH; i++) {
    my_print2(mac[i], HEX);
    if (i<WL_MAC_ADDR_LENGTH-1) my_print(":");
  }
  my_print("\n");
  delay(500);

  // try all known ssids in order forever until one works
  while (WiFi.status() != WL_CONNECTED) {

    for (int i=0; i<num_known_ssids && WiFi.status() != WL_CONNECTED; i++) {
      int t_start = millis();
      ssid = known_ssids[i].ssid;
      password = known_ssids[i].password;
      my_clear();
      my_print("Trying: "); my_println(ssid);
      
      if (password[0]) {
        WiFi.begin(ssid, password);
      } else {
        WiFi.begin(ssid);
      }
    
      // Wait for connection
      while (WiFi.status() != WL_CONNECTED) {
        if (millis()-t_start >= WIFI_TIMEOUT) {
          my_print("Timeout\n");
          delay(500);
          break;
        }
        my_print(".");
        delay(500);
  
      }
    }
  }
  
  my_clear();
  my_print("SSID: ");
  my_println(ssid);
  my_print("IP: ");
  my_println(WiFi.localIP());

  delay(2000);

  do_update();
}

void loop() {
  int time_now = millis();
  if (digitalRead(pin_button)==0 || time_now - time_last_update >= UPDATE_RATE) {
    time_last_update = time_now;
    do_update();
    delay(1000);
  }
  
  delay(100);

}


