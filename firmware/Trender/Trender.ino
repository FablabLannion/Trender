/**
 *  This file is part of Trender.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2016 Jérôme Labidurie jerome@labidurie.fr
 *  Copyright 2016 Cédric Bernard cedric.bernard@galilabs.com
 *  Copyright 2016 Julien Jacques julien.jacques@galilabs.com
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <EEPROM.h>

#define HOSTNAME "Trender"
#define mySSID "Trender"
#define VERSION "Trender v1.0"

ESP8266WebServer server(80);
Ticker tk, tki, tkb;
/** base rate for heartbeat ticker */
#define TKB_BASE_RATE 0.10
/** base rate for main ticker */
#define TK_BASE_RATE 1
volatile boolean showRainbow = false;
volatile uint32_t color = 0;

typedef struct t_config {
  uint8_t dur;              /**< total duration (min) */
  uint32_t colors[3];       /**< colors to display */
  uint8_t  per[3];          /**< threasholds, last one is always 100 */
  uint8_t hb;               /**< heartbeat mode */
} T_CONFIG;
volatile T_CONFIG config;

/*** timer variables ***/
unsigned long startTime = 0;/**< begining time of trender mode 0 */
uint8_t previousState = HIGH;
/** current/previous timer mode
 * will be 0 for 1st color, 1 then 2
 * can also be STOPPED
 */
#define STARTED 0
#define STOPPED 255
uint8_t previousMode = STOPPED;
uint8_t currentMode = STOPPED;
/** ending time (in millis()) of mode x
 */
#define END_TIME_MODE(x) ( startTime + (config.dur*60000) * config.per[x] / 100 )
/** begining time (in millis()) of mode x
 */
#define START_TIME_MODE(x) ( (x==0)? startTime : END_TIME_MODE(x-1) )

/** include web pages */
#include "Page_Color.h"
#include "Page_Config.h"
#include "Page_Admin.h"
#include "Page_Script_js.h"
#include "Page_Style_css.h"
#include "Page_Information.h"
#include "favicon.h"

#define PIN D2
#define PIN_INPUT D5
#define NBPIX 3

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
  Serial.println(VERSION);
  WiFi.hostname (HOSTNAME);
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID);
  Serial.println(WiFi.softAPIP());

  server.on ( "/", []() { Serial.println("/.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/admin", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/admin/infovalues", send_information_values_html);
  server.on ( "/info", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
//   server.on ( "/color", processColor);
//   server.on ( "/color/values", sendColorData);
  server.on ( "/config", []() { processConfig(); writeConfig();});
  server.on ( "/config/values", sendConfigData);
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/start", []() { Serial.println("start"); start(); server.send ( 200, "text/html", PAGE_AdminMainPage ); } );
  server.on ( "/stop", []() { Serial.println("stop"); stop(); server.send ( 200, "text/html", PAGE_AdminMainPage ); } );
  server.on ( "/favicon.ico", []() { Serial.println("favicon"); server.send_P(200, "image/x-icon", favicon_ico, favicon_ico_len); } );
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 404, "text/html", "Page not Found" );   }  );

  // config init
  config.dur = 1;
  config.colors[0] = 0x00feff;
  config.colors[1] = 0xfffe00;
  config.colors[2] = 0xff0000;
  config.per[0] = 50;
  config.per[1] = 90;
  config.per[2] = 100;
  config.hb = 0;
  EEPROM.begin(512);
  readConfig();

  server.begin();
  Serial.println( "HTTP server started" );

  pinMode (PIN_INPUT, INPUT);
  tki.attach(0.1, tkInput);

    setColor(0xff0000);
    delay(300);
    setColor(0x00ff00);
    delay(300);
    setColor(0x0000ff);
    delay(300);
    setColor(0);


  //tk.attach(0.05, tkColor);
}


/** main loop
* handle http events
*/
void loop() {
  server.handleClient();
}


/** ticker callback for rainbow mode
* NOT USED anymode
*/
void tkColor() {
  uint16_t i;
  static uint16_t j=0;

  if (!showRainbow) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
  } else {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(j));
    }
    j = (j+1) % 255;
  }
  strip.show();
}

/** read input PIN_INPUT
 */
void tkInput () {
   uint8_t r;

   r = digitalRead(PIN_INPUT);
   //Serial.println(r);
   if ((r == HIGH) && (previousState == LOW)) {
      if (currentMode != STOPPED) {
         stop();
      } else {
         start();
      }
   }
   previousState = r;
}//tkInput

/** set all pixel to color
 * @param col the color as 0xRRGGBB
 */
void setColor (uint32_t col) {
    uint8_t i=0;

//     Serial.println(strip.getPixelColor(0),HEX);
//     Serial.println(col,HEX);

    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, col);
    }
	strip.setBrightness (255);
    strip.show();
}

/** ticker callback for heartbeat
 */
void tkHeartbeat(void) {
    static uint16_t bright = 255;///< current brightness
    static uint8_t decrement = 1;///< going up or down

    if (decrement) {
        strip.setBrightness (bright);
        bright -= 20;
        if (bright <= 50) decrement = 0;
    } else {
        strip.setBrightness (bright);
        bright += 20;
        if (bright >= 255) decrement = 1;
    }
    strip.show();
}

/** ticker callback for trender main timer
 */
void tkTrender(void) {
    unsigned long now = millis();
    unsigned long quater_length = 0;
    uint8_t quater = 0;

    // compute the mode
    if (now < (startTime + (config.dur*60000)*config.per[0]/100)) {
        previousMode = currentMode;
        currentMode = 0;
    } else
    if (now < (startTime + (config.dur*60000)*config.per[1]/100 )) {
        previousMode = currentMode;
        currentMode = 1;
    } else
    if (now < (startTime + (config.dur*60000))) {
        previousMode = currentMode;
        currentMode = 2;
    } else {
        stop();
        return; // exit now, next computation (hb) will divide by 0 (quater_length will be == 0)
    }
    if (previousMode != currentMode) {

        setColor (config.colors[currentMode]);
        previousMode = currentMode;
    }
    /* if heartbeat is activated, compute in which quater we are
     * set heartbeat ticker acordingly
     */
	/*
	// quarter1: one pulse
	// quarter2: two pulse
	// quarter3: three pulse
	// quarter4: four pulse
    if (config.hb) {
        quater_length = ( END_TIME_MODE(currentMode) - START_TIME_MODE(currentMode) ) / 4;
        quater = (now - START_TIME_MODE(currentMode) ) / quater_length;

        Serial.print(currentMode);Serial.print(",");
        Serial.print(START_TIME_MODE(currentMode));Serial.print(",");
        Serial.print(END_TIME_MODE(currentMode) );Serial.print(",");
        Serial.print(quater_length);Serial.print(",");
        Serial.println(quater);
        tkb.attach( ( TKB_BASE_RATE / (currentMode+1) ) / (quater+1) , tkHeartbeat);
    }
	*/

	// quarter1: no pulse
	// quarter2: no pulse
	// quarter3: one pulse
	// quarter4: three pulse
    if (config.hb) {
        quater_length = ( END_TIME_MODE(currentMode) - START_TIME_MODE(currentMode) ) / 4;
        quater = (now - START_TIME_MODE(currentMode) ) / quater_length;
        Serial.print(currentMode);Serial.print(",");
        Serial.print(START_TIME_MODE(currentMode));Serial.print(",");
        Serial.print(END_TIME_MODE(currentMode) );Serial.print(",");
        Serial.print(quater_length);Serial.print(",");
        Serial.println(quater);
		switch (quater) {
			case 2:
			  tkb.attach( ( TKB_BASE_RATE / (currentMode+1) ) / (quater-1) , tkHeartbeat);
			  break;
			case 3:
			  tkb.attach( ( TKB_BASE_RATE / (currentMode+1) ) / (quater) , tkHeartbeat);
			  break;
			default:
			  tkb.detach();
			break;
		  }






    }


} // tkTrender

/** start main timer
 */
void start (void) {
    // flash to show the 3 colors
    setColor(config.colors[0]);
    delay(500);
    setColor(config.colors[1]);
    delay(500);
    setColor(config.colors[2]);
    delay(500);
    setColor(0);
    // init timer
    startTime = millis();
    Serial.print("START ");
    Serial.println(startTime);
    previousMode = currentMode = STOPPED;
    tk.attach(TK_BASE_RATE, tkTrender);
    if (config.hb) {
        //tkb.attach(TKB_BASE_RATE, tkHeartbeat);
    }
} // start

/** stop main timer
 */
void stop (void) {
    setColor(0);
    currentMode = previousMode = STOPPED;
    tk.detach();
    if (config.hb) {
        tkb.detach();
    }
    Serial.println("STOP");
}//stop

/** read config from eeprom
 */
void readConfig (void) {
    int i = 0, j = 0;
    uint8_t c;

    // check magic marker
    c = EEPROM.read (i++);
    Serial.println(c,HEX);
    if (c != 'G') return;
    c = EEPROM.read (i++);
    Serial.println(c,HEX);
    if (c != 'A') return;
    c = EEPROM.read (i++);
    Serial.println(c,HEX);
    if (c != 'L') return;

    config.dur = EEPROM.read (i++);
    for (j = 0; j < 3; j++) {
      config.colors[j]  = ((uint32_t)EEPROM.read (i++)) << 16;
      config.colors[j] |= ((uint32_t)EEPROM.read (i++)) << 8;
      config.colors[j] |= ((uint32_t)EEPROM.read (i++));
    }
    for (j = 0; j < 2; j++) {
      config.per[j] = EEPROM.read (i++);
    }
    config.hb = EEPROM.read (i++);
}// readConfig

/** write config to eeprom
 */
void writeConfig (void) {
  int i = 0, j = 0;

  // write magic marker
  EEPROM.write(i++,'G');
  EEPROM.write(i++,'A');
  EEPROM.write(i++,'L');

  EEPROM.write(i++,config.dur);

    for (j = 0; j < 3; j++) {
      EEPROM.write(i++,config.colors[j]>>16);
      EEPROM.write(i++,(config.colors[j]&0x00FF00)>>8);
      EEPROM.write(i++,(config.colors[j]&0x0000FF));
    }
    for (j = 0; j < 2; j++) {
      EEPROM.write(i++,config.per[j]);
    }
    EEPROM.write(i++, config.hb);
    EEPROM.commit();
}// writeConfig

/** Input a value 0 to 255 to get a color value.
* The colours are a transition r - g - b - back to r.
* @param WheelPos current whell position 0-255
*/
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/** Go to designated color
 *
 * Fade from current color. Each component (R,G,B) are (inc|dec)remented
 * by one step up to the given color.
 *
 * @param color to go
 * @param wait delay (ms) between each step
 */
void gotoColor (uint32_t color, uint8_t wait)
{
   uint32_t cColor = strip.getPixelColor(0);  /** current color */
   uint8_t rc = cColor >> 16;               /** current red */
   uint8_t gc = cColor >> 8 & 0xFF;         /** current green */
   uint8_t bc = cColor & 0xFF;              /** current blue */
   int8_t  ri=1,gi=1,bi=1;                  /** inc/dec for each component */

   if (color == cColor)
       return;

   // compute inc or dec for each component
   if ( (rc) > (color>>16) ) {
      ri = -1;
   }
   if ( (gc) > (color>>8&0xFF) ) {
      gi = -1;
   }
   if ( (bc) > (color&0xFF) ) {
      bi = -1;
   }

   // goto
   while (cColor != color) {
      if (rc != (color>>16))
         rc += ri;
      if (gc != (color>>8&0xFF))
         gc += gi;
      if (bc != (color&0xFF))
         bc += bi;
      cColor = strip.Color (rc,gc,bc);
      setColor (cColor);
      delay(wait);
   }
} // gotoColor

