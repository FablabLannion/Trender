/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 *
 *  modified by Julien JACQUES - Fablab de Lannion
 *
 *  author: jjacques@legtux.org
 *  date  : 19-oct-2016
 *  desc  : connect to thingspeak.com 
 *          retrieve the last value of channel identified by ThingSpeakChannelId
 *          communication with NeoPixel strip to modify its color value
 *
 *  notes : complete the "........" before compile this sketch
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>

#define PIN D2
#define NBPIX 3


const char* ssid = "........";
const char* password = "........";

volatile uint32_t color = 0;

const uint32_t delay_get_samples = 15; /* time (unit:second) between two sample requests */

//const char* host = "api.github.com";
const char* host   = "thingspeak.com";
const char* ThingSpeakChannelId = "........";

const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";
const char* fingerprint = "7860 18 44 81 35 BF DF 77 84 D4 0A 22 0D 9B 4E 6C DC 57 2C";


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBPIX, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  color=strip.Color(0,255,0);
  strip.setPixelColor(0, color);
  strip.show();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  color=strip.Color(255,255,0);
  strip.setPixelColor(0, color);
  strip.show();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  delay(2000);
  color=strip.Color(0,0,0);
  strip.setPixelColor(0, color);
  strip.show();
}


void loop() {
  uint16_t i;
  
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

//  String url = "/repos/esp8266/Arduino/commits/master/status";
  String url = "/channels/";
  url += ThingSpeakChannelId;
  url += "/fields/1/last";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');
/*
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
*/
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  
  Serial.println("==========");
  Serial.println("closing connection");

  // update color
  switch(line.toInt()) {
  case 0:
    Serial.println("=> red");
    color=strip.Color(255,0,0);
    break;
  case 1:
    Serial.println("=> blue");
    color=strip.Color(0,0,255);
    break;
  case 2:
    Serial.println("=> green");
    color=strip.Color(0,255,0);
    break;
  default:
    Serial.println("=> off");
    color=strip.Color(0,0,0);
  }

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();

  
  Serial.print("waiting for ");
  Serial.print( delay_get_samples );
  Serial.print(" second");
  if(delay_get_samples>1)
    Serial.println(" s");
  else
    Serial.println("");
    
  delay(delay_get_samples*1000);
}



