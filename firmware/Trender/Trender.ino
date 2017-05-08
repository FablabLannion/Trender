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
#include <DNSServer.h>

#define HOSTNAME "Trender"
#define baseSSID "Trender-"
#define VERSION "Trender v1.0"
char mySSID[13];



 /**********************************************************/
 /****            THINGSPEAK CONFIGURATION              ****/
 /**********************************************************/
const char* ThingSpeakServer = "api.thingspeak.com";  // server's address
const unsigned char  ThingSpeakPort = 80; 

//thinkgspeak API KEY for Trenders. R,G,B will be retrieve from this channel
//TODO : this data shall be modified by dedicated configuration page
const char* channelID = "*****";   

unsigned char thingSpeakMode = 0;
/**
0: Accepoint mode ( configuration, timekeeper ) , 
1: Thingspeak mode, init mode. Close AP, open Wifi to connect to thingspeak server
2: wifi established, import RGB values and update neopixels accordingly 
*/


 /**********************************************************/
 /****            JSON PARSER CONFIGURATION             ****/
 /**********************************************************/
#include <ArduinoJson.h>

// resource1 & resource2 are constant values to be used to parse json file computed by thingspeal
const char* resource1 = "/channels/";   // http resource screened 
const char* resource2 = "/feeds/last.json"; 
const unsigned long HTTP_TIMEOUT = 20000;  // max response time from server
const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response

// The type of data that we want to extract from the page
struct UserData {
  char created_at[128]; 
  unsigned long entry_id;
  unsigned char field1; 
  unsigned char field2; 
  unsigned char field3;  
  unsigned char field4;   
  unsigned char field5;   
  unsigned char field6;   
  unsigned char field7;   
};




unsigned char newSampleDetected=0;
char timestamp[32]="dummydata";
StaticJsonBuffer<200> jsonBuffer;


 /**********************************************************/
 /****            WIFI CONFIGURATION                ****/
 /**********************************************************/
 

 
//////////////////////
// WiFi Definitions //
//////////////////////
char ssid[] = "***";      //  your network SSID (name) // TODO : shall be configurable from webserver
char pass[] = "**";   // your network password // TODO : shall be configurable from webserver

 

#define NB_TRY 10 /**< duration of wifi connection trial before aborting (seconds) */
int status = WL_IDLE_STATUS;
IPAddress myIp;
WiFiClient client;


 /**********************************************************/
 /****            Access Point CONFIGURATION            ****/
 /**********************************************************/



ESP8266WebServer server(80);
Ticker tk, tki, tkb,tkt;
/** base rate for heartbeat ticker */
#define TKB_BASE_RATE 0.10
/** base rate for main ticker */
#define TK_BASE_RATE 1
/** base rate for timeout ticker ( tkt) */
#define TK_BASE_TIMEOUT 30

volatile boolean showRainbow = false;
volatile uint32_t color = 0;

DNSServer dnsServer;
#define DNS_PORT 53 /**< DNS port */

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
#include "Page_ThingSpeak.h"
#include "favicon.h"




 /**********************************************************/
 /****            NEOPIXEL CONFIGURATION                ****/
 /**********************************************************/



#define PIN D2
#define PIN_INPUT D5
#define NBPIX 16 // take into account several neopixels footprints: 1 pixel, 1 row of 8 pixels, 1 square of 4*4=16 pixels





// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NBPIX, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


//Following data will be updated once json file is parsed
uint8_t R=0;
uint8_t G=0;
uint8_t B=0;

uint8_t brightness = 255; //sets the default brightness value

void setup() {
  uint8_t mac[6];
  
  WiFi.macAddress(mac);
  snprintf (mySSID, 13, "%s%02X%02X", baseSSID, mac[4], mac[5]);
  
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

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
  server.on ( "/admin/infothingspeak", send_thingspeak_values_html);
  
  server.on ( "/info", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
//   server.on ( "/color", processColor);
//   server.on ( "/color/values", sendColorData);
  server.on ( "/config", []() { processConfig(); writeConfig();});
  server.on ( "/thingspeak", []() { Serial.println("thingspeak.html"); server.send ( 200, "text/html", PAGE_ThingSpeak );   }  );  
  server.on ( "/config/values", []() { Serial.println("Disable ThingSpeak timeout mode"); thingSpeakMode=0;tkt.detach();sendConfigData();});
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

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start ( DNS_PORT, "*", WiFi.softAPIP() );
  // webserver
  server.begin();
  Serial.println( "HTTP server started" );

  pinMode (PIN_INPUT, INPUT);
  tki.attach(0.1, tkInput);
  tkt.attach(TK_BASE_TIMEOUT, tkCheckMode); 

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
	
	
		switch (thingSpeakMode) {
			case 0:
			   //Access point mode
			  dnsServer.processNextRequest();
			  server.handleClient();				
			  break;
			case 1:
				Serial.println("Close AP and open Connection to thingspeak server");
				Serial.println("ThingSpeakMode enabled");
				setColor(0xff0000);
				delay(300);
				setColor(0x0000ff);
				delay(300);
				setColor(0);
				setupWiFi();
			  break;			  
			case 2:
			 
				//------------------------------------ 
				// Interaction with ThingSpeak  
				//------------------------------------	
				// Retrieve latest value from ThingSpeak Database
				if (client.connect(ThingSpeakServer,ThingSpeakPort)) {
							Serial.println("connected to thingspeak");  
					
							if (sendRequest(ThingSpeakServer) && skipResponseHeaders() && skipThingSpeakHeader() ) {
							  char response[MAX_CONTENT_SIZE];
							   delay(2000);
							  readReponseContent(response, sizeof(response));
							  UserData userData;
							  if (parseUserData(response, &userData)) {
								printUserData(&userData);
							  }
							}
							
						}
						else{
							Serial.println("ThingSpeakServer connection error");
						}
						delay(2000); 
			  break;
			default:
			break;
		  }	
	
}


/** ticker callback for rainbow mode
* NOT USED anymore
*/
void tkColor() {
  uint16_t i;
  static uint16_t j=0;

  if (!showRainbow) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, color);
    }
  } else {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(j));
    }
    j = (j+1) % 255;
  }
  pixels.show();
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



/** Check if we need to stay into landing page of force Thingkspeak connector
 */
 void tkCheckMode () {

  Serial.println("Timeout reached, force ThingspeakMode");
  thingSpeakMode=1;
  tkt.detach();

}//tkCheckMode



/** set all pixel to color
 * @param col the color as 0xRRGGBB
 */
void setColor (uint32_t col) {
    uint8_t i=0;

//     Serial.println(pixels.getPixelColor(0),HEX);
//     Serial.println(col,HEX);

    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, col);
    }
	pixels.setBrightness (255);
    pixels.show();
}

/** ticker callback for heartbeat
 */
void tkHeartbeat(void) {
    static uint16_t bright = 255;///< current brightness
    static uint8_t decrement = 1;///< going up or down

    if (decrement) {
        pixels.setBrightness (bright);
        bright -= 20;
        if (bright <= 50) decrement = 0;
    } else {
        pixels.setBrightness (bright);
        bright += 20;
        if (bright >= 255) decrement = 1;
    }
    pixels.show();
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
	// Disable timeout timer if any
	tkt.detach();
	  
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





 /**********************************************************/
 /****            NEOPIXEL FUNCTIONS                    ****/
 /**********************************************************/


//-------------------------------------------------------
void writeLEDS(byte R, byte G, byte B)//basic write colors to the neopixels with RGB values
//-------------------------------------------------------
{
	
	#ifdef DEBUG_MODE
	 Serial.print("R:");
	 Serial.println(R);
	 Serial.print("G:");
	 Serial.println(G);
	 Serial.print("B:");
	 Serial.println(B);	 
	#endif
	
  for (int i = 0; i < pixels.numPixels(); i ++)
  {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
  }
  pixels.show();
}

//-------------------------------------------------------
void writeLEDS(byte R, byte G, byte B, byte bright)//same as above with brightness added
//-------------------------------------------------------
{
  float fR = (R / 255) * bright;
  float fG = (G / 255) * bright;
  float fB = (B / 255) * bright;
  for (int i = 0; i < pixels.numPixels(); i ++)
  {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
  }
  pixels.show();
}





/** Input a value 0 to 255 to get a color value.
* The colours are a transition r - g - b - back to r.
* @param WheelPos current whell position 0-255
*/
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
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
   uint32_t cColor = pixels.getPixelColor(0);  /** current color */
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
      cColor = pixels.Color (rc,gc,bc);
      setColor (cColor);
      delay(wait);
   }
} // gotoColor


//-------------------------------------------------------
// WIFI CLIENT FUNCTIONS
//-------------------------------------------------------



//-------------------------------------------------------
/** connect to an existing wifi network
 *  @param[in] ssid the ssid of the network
 *  @param[in] pwd the password of the network
 *  @return assigned ip address
 */
IPAddress wicoSetupWifi(char* ssid, char* pwd) {
//-------------------------------------------------------
    // try connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pwd);
  for (int i=0; i<2*NB_TRY; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      return WiFi.localIP();
    }
    Serial.print(WiFi.status());
    delay(500);
	//Twist for a while to show we are still looking for wifi
	   for (int i=0; i <= 3; i++){
			//  White	 Band
			R=128;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);
			R=0;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);		
	   } 	
	   
	   if (status != WL_CONNECTED) {
		   Serial.println("Wifi not connected :-( ");		   
	   }

  }
  return IPAddress (0,0,0,0);
}





//-------------------------------------------------------
void setupWiFi()
//-------------------------------------------------------
{


  myIp = wicoSetupWifi (ssid, pass);
  if (myIp != IPAddress(0,0,0,0)) {
    // success
	Serial.println("Successfully Connected to WIFI :-)");  
	//Blue Color
	
	R=0;
	G=0;
	B=255;	
	writeLEDS(R, G, B); 
	delay(500);	
	//Breath();	
	//Black Color
	R=0;
	G=0;
	B=0;	
	writeLEDS(R, G, B);      
	delay (50);	
	

    Serial.println("Wifi Successfully Configured, IP: ");  
	Serial.println(myIp); 

	//Ready for Next State in main loop
	thingSpeakMode=2;		
	

 	
  } 
  else
  {
	Serial.println("Failed connecting to WIFI, go back to Access Point to adapt Wifi parameters "); 
	ESP.restart();//Same results as ESP.reset() // TODO: to be enhanced, this does not reboot properly, stuck in "wdt reset" mode :-(
	//thingSpeakMode=5;

  }
 

 


}








 /**********************************************************/
 /****            JSON FUNCTIONS                        ****/
 /**********************************************************/




// Send the HTTP GET request to the server
//---------------------------------------- 
bool sendRequest(const char* host) {
//---------------------------------------- 
  
  Serial.println("Is there anybody out there?");
  //Serial.println(resource);
  

  client.print("GET ");
  
  client.print(resource1);
  client.print(channelID);
  client.print(resource2);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  return true;
}




// Skip HTTP headers so that we are at the beginning of the response's body
//---------------------------------------- 
bool skipResponseHeaders() {
//---------------------------------------- 
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
	
	//Twist for a while to show we found something wrong
	   for (int i=0; i <= 10; i++){
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);
			//  White	 Band
			R=0;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);		
	   } 
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   	
  }

  return ok;
}

// Skip HTTP headers so that we are at the beginning of the response's body
//-------------------------------------------------------
bool skipThingSpeakHeader() {
//-------------------------------------------------------


  // Flush first line to go to align to json body
  char endOfHeaders[] = "\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("ThingSpeak Header not found :-(");
	//Twist for a while to show we found something wrong
	   for (int i=0; i <= 10; i++){
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);
			//  White	 Band
			R=0;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);		
	   } 
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   	   
	   
  }



  return ok;
}



// Read the body of the response from the HTTP server
//-------------------------------------------------------
void readReponseContent(char* content, size_t maxSize) {
//-------------------------------------------------------
  size_t length = client.readBytes(content, maxSize);
  content[length] = 0;
  Serial.println(content);
}



// Parse the JSON from the input string and extract the interesting values
//-------------------------------------------------------
bool parseUserData(char* content, struct UserData* userData) {
//-------------------------------------------------------
  // Compute optimal size of the JSON buffer according to what we need to parse.
  // This is only required if you use StaticJsonBuffer.
  const size_t BUFFER_SIZE =
      JSON_OBJECT_SIZE(10);     // the root object has 10 elements
/*
{"created_at":"2017-05-08T11:27:12Z","entry_id":3397,"field1":"90","field2":"0","field3":"0","field4":null,"field5":null,"field6":null,"field7":null,"field8":null}
*/



  // Allocate a temporary memory pool on the stack
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  // If the memory pool is too big for the stack, use this instead:
  // DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(content);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }

  // Here were copy the strings we're interested in
  
  strcpy(userData->created_at, root["created_at"]);  
  userData->field1=atoi(root["field1"]);
  userData->field2=atoi(root["field2"]);
  userData->field3=atoi(root["field3"]);


  return true;
}

// Print the data extracted from the JSON
//-------------------------------------------------------
void printUserData(const struct UserData* userData) {
//-------------------------------------------------------

  
  Serial.print("old timestamp: ");
  Serial.println(timestamp);
  
 
  Serial.print("created_at = ");
  Serial.println(userData->created_at);
  Serial.print("Field1 = ");
  Serial.println(userData->field1);
  Serial.print("Field2 = ");
  Serial.println(userData->field2);
  Serial.print("Field3 = ");
  Serial.println(userData->field3); 

  
  //Check If this is a new sample
  if (strcmp(timestamp,userData->created_at)  != 0)
  {
    Serial.println("Hey Dude! We have something new here, let's rock!");
	newSampleDetected=1;
	
	
	R=userData->field1;
	G=userData->field2;
	B=userData->field3;	


	strcpy(timestamp,userData->created_at);
	

	
	
	writeLEDS(R, G, B); //TODO : this should be done elsewhere, to control neopixel in a dedicated thread!


	
	
  }
  else
  { 
    Serial.println("go to bed...");
	newSampleDetected=0;	

  }  

	  
}


