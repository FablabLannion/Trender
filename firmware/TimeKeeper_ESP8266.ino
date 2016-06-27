 /* Demo Project for TimeKeeper.
 *  Mix of several projects from Fablab Lannion and SweetTimeBox from  Nokia-Lannion 
 * Connections:
 * WeMos D1 Mini   ConnectTo    Description
 * (ESP8266)       
 *
 * D2              In            Cmd for Neopixel. Use a 479ohm resistor between IO & neopixel input
 * D5              SELECTOR_HIGH 3.3V Output from ESP to switch  ( right pad from switch ) 
 * D6              SELECTOR_I    Input from switch  to   ESP ( middle pad from switch ) 
 * 3V3             Vcc           3.3V from ESP to neopixel
 * D0 (GPIO16)     BL            3.3V to turn backlight on, or PWM
 * G               Gnd           Ground for neopixel &  left pad from switch 
 
// https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/example-sketch-ap-web-server
//Adress for access point : http://192.168.4.1/ , to be confirmed using serial line from arduino IDE.

//Environment to be used to build the project:
// ARDUINO 1.6.9
// Board : see http://www.banggood.com/3Pcs-D1-Mini-NodeMcu-Lua-WIFI-ESP8266-Development-Board-p-1047943.html for details

*/

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

 /**********************************************************/
 /****            TIMEKEEPER CONFIGURATION             ****/
 /**********************************************************/
 unsigned int time1=30; //in seconds
 unsigned int time2=50;
 unsigned int time3=60;
 unsigned int nbOfSecondsElapsed=0;
 unsigned char stepNumber=0;
 unsigned int abortCycle=0;
 
unsigned char colorsKeeper[][3] = { {0,0,255},
                    {200,255,4},
					{255,0,0},{0,0,0}};
					
//Default profiles:

// #1: 1mn : 50%, 90%
// #2: 10mn : 50%,90%
// #3: 1H   : 40mn,55mn
unsigned int profilesKeeper[][3] = { {30,54,60},{300,540,600},{2400,3300,3600}};	
#define kDynamicProfile 2 // id where the profile updated by webserver will be overridden
unsigned char selectedProfile=0;				
					
int SELECTOR_HIGH = D5; 
int SELECTOR_I = D6; 
int selector =0; 
int selector_previous_off =1;
int selector_previous_on =0;
int selector_previous =0;
			
				
					

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "timekeeper"; //Wifi password for access point
IPAddress myIp;
ESP8266WebServer wicoServer(80);
int  wicoConfigAddr = 0; /**< address in eeprom to storetime profiles */
uint8_t wicoIsConfigSet = 0; /**< is wifi configuration has been set through webserver ? */



/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = BUILTIN_LED; // Thing's onboard, green LED





 /**********************************************************/
 /****            NEOPIXEL CONFIGURATION                ****/
 /**********************************************************/



#include <Adafruit_NeoPixel.h>
#define PIN            D2 //sets the pin on which the neopixels are connected
#define NUMPIXELS      3 //defines the number of pixels in the strip
#define interval       50 //defines the delay interval between running the functions
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255, 0, 0);
uint32_t blue = pixels.Color(0, 0, 255);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t pixelColour;
uint32_t lastColor;
float activeColor[] = {255, 0, 0};//sets the default color to red
boolean NeoState[] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, true}; //Active Neopixel Function (off by default)
unsigned long lastConnectionTime = 0;            // last time brightness was updated, in milliseconds
const unsigned long postingInterval = 1000L * 60L; // delay between updates, in milliseconds
uint8_t brightness = 255; //sets the default brightness value
uint8_t R=0;
uint8_t G=0;
uint8_t B=0;




//-------------------------------------------------------
// WIFI FUNCTIONS
//-------------------------------------------------------

/** read wifi config stored in eeprom.
 *  
 *  There must be at least 35+64 bytes after given configAddr.
 *  returned value should be configAddr+32+32+32
 *  eeprom must have been initialised : EEPROM.begin(size);
 *  
 *  @param[in] configAddr eeprom address for begining of configuration
 *  @param[out] time1 read time1 in seconds : 32 Bytes
 *  @param[out] time2 read time2 in seconds : 32 Bytes
 *  @param[out] time3 read time3 in seconds : 32 Bytes 
 *  @return 1 if success, 0 if failure
 */
//------------------------------------------------------- 
int wicoReadWifiConfig (int configAddr, char* time1, char* time2, char* time3) {
//-------------------------------------------------------
  int i = 0;
  
  for (i = 0; i < 32; i++)
    {
      time1[i] = char(EEPROM.read(i+configAddr));
    }
  for (i=0; i < 32; i++)
    {
      time2[i] = char(EEPROM.read(i+configAddr+32));
    }
  for (i=0; i < 32; i++)
    {
      time3[i] = char(EEPROM.read(i+configAddr+64));
    }	
    return 1;
}



/** write wifi config to eeprom.
 *  
 *  There must be at least 35+64 bytes after given configAddr.
 *  returned value should be configAddr+32+64
 *  eeprom must have been initialised : EEPROM.begin(size);
 *  commit() is done here
 *  
 *  @param[in] configAddr eeprom address for begining of configuration
 *  @param[in] ssid read ssid : 32 Bytes
 *  @param[in] pwd read wifi password : 64 Bytes
 *  @return 1 if success, 0 if failure
 */
int wicoWriteWifiConfig (int configAddr, const char time1[32], const char time2[32], const char time3[32]) {
  int i = 0;
  
  for (i = 0; i < 32; i++)
    {
      EEPROM.write(i+configAddr,time1[i]);
    }
  for (i=0; i < 32; i++)
    {
      EEPROM.write(i+configAddr+32,time2[i]);
    }
  for (i=0; i < 32; i++)
    {
      EEPROM.write(i+configAddr+64,time3[i]);
    }	
    EEPROM.commit();
    return 1;
}



//-------------------------------------------------------
IPAddress setupWiFi()
//-------------------------------------------------------
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "SweetTime " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);
Serial.println("AP_NameChar:");
Serial.println(AP_NameChar);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  return  WiFi.softAPIP();  
}


/** reset wifi config in eeprom
 *  
 *  write \0 all over the place.
 *  
 *  @param[in] configAddr eeprom address for begining of configuration
 *  @return 1 if success, 0 if failure
 */
int wicoResetWifiConfig (int configAddr) {
  char d[64];
  memset (d, 0, 64);
  return wicoWriteWifiConfig (configAddr, d, d, d);
  
}



/** handle / URI for AP webserver
 */
//------------------------------------------------------- 
void wicoHandleRoot (void) {
//-------------------------------------------------------
  String s = "<html><body><h1>SweetTime Config</h1>";

  // arguments management
  if (wicoServer.hasArg("reset") ) {
    Serial.println("reset");
    wicoResetWifiConfig (wicoConfigAddr);
  } else if (wicoServer.hasArg("time1")) {
    Serial.print("save:");
    Serial.println(wicoServer.arg("time1").c_str());
    Serial.print("save:");
    Serial.println(wicoServer.arg("time2").c_str());
    Serial.print("save:");
    Serial.println(wicoServer.arg("time3").c_str());

	Serial.println("**** BEFORE:*****");
	Serial.print("time1:");
	Serial.println(time1);
	Serial.print("time2:");
	Serial.println(time2);
	Serial.print("time3:");
	Serial.println(time3);	
	
	time1 = atoi(wicoServer.arg("time1").c_str());

	time2 = atoi(wicoServer.arg("time2").c_str());

	time3 = atoi(wicoServer.arg("time3").c_str());	
	
	
	Serial.println("**** AFTER:*****");
	Serial.print("time1:");
	Serial.println(time1);
	Serial.print("time2:");
	Serial.println(time2);
	Serial.print("time3:");
	Serial.println(time3);	
	//Refresh profilesKeeper
     profilesKeeper[kDynamicProfile][0]=time1;
     profilesKeeper[kDynamicProfile][1]=time2;
     profilesKeeper[kDynamicProfile][2]=time3;
 	
	
    wicoWriteWifiConfig (wicoConfigAddr, wicoServer.arg("time1").c_str(), wicoServer.arg("time2").c_str(), wicoServer.arg("time3").c_str());
    wicoIsConfigSet = 1;
  }

  // construct <form>
  s += "<p><form>Time1 in s: ";

  s += "<input type=text name=time1>";

  s += "<br>Time2 in s: <input type='text' name='time2'><br/><br>Time3 in s: <input type='text' name='time3'><br/><br/><input type='submit' value='send'></form></p></body></html>\n";
  
  wicoServer.send ( 200, "text/html", s );
}

/** start web server and wait for wifi configuration.
 *  @return only when wifi configuration has been set
 */
//------------------------------------------------------- 
void wicoSetupWebServer (void) {
//-------------------------------------------------------
  wicoIsConfigSet = 0;
  wicoServer.on("/", wicoHandleRoot);
  wicoServer.begin();
  Serial.println("HTTP server started");
  while (!wicoIsConfigSet) {
    wicoServer.handleClient();
  }
  wicoServer.stop();
}





/** configure wifi.
 *  read recorded informations from eeprom
 *  try to connect
 *  if failure, start AccessPoint
 *  @param(in] configAddr config data address
 *  @param[in] apSsid AP ssid name to be used
 *  @param[out] myIp affected (wifi or AP) IP address
 *  @return 1 if wifi connection is ok, 0 is AP has been started
 */
//------------------------------------------------------- 
int wicoWifiConfig (int configAddr) {
//------------------------------------------------------- 
  char time1_[32];
  char time2_[32];  
  char time3_[32];

  wicoConfigAddr = configAddr;

  // read config
  wicoReadWifiConfig (configAddr, time1_, time2_, time3_);
  Serial.print(time1_);
  Serial.print("/");
  Serial.print(time2_);
  Serial.println("/");
  Serial.print(time3_);
  Serial.println("/"); 
  
	
	time1 = atoi(time1_);
	time2 = atoi(time2_);
	time3 = atoi(time3_);	


	Serial.println("**** AFTER:*****");
	Serial.print("time1:");
	Serial.println(time1);
	Serial.print("time2:");
	Serial.println(time2);
	Serial.print("time3:");
	Serial.println(time3);	
	//Refresh profilesKeeper
	 profilesKeeper[kDynamicProfile][0]=time1;
	 profilesKeeper[kDynamicProfile][1]=time2;
	 profilesKeeper[kDynamicProfile][2]=time3;  

  return 0;
}


//-------------------------------------------------------
// MAIN FUNCTIONS
//-------------------------------------------------------





//-------------------------------------------------------
void initHardware()
//-------------------------------------------------------
{
  EEPROM.begin(512);
  Serial.begin(9600); // initialize serial communication

  Serial.println("TimeKeeper initiated");

  //Load Dynamic Profile Values
  wicoWifiConfig (0);

  pixels.begin(); //starts the neopixels
  pixels.setBrightness(brightness); // sets the inital brightness of the neopixels
  pixels.show();
  delay(1000);  

	  
  //Switch used to start/stop sweettimme or select time mode
  pinMode(SELECTOR_I, INPUT);      // set the Switch pin mode 	 
  selector_previous=selector;
  selector = digitalRead(SELECTOR_I);   // read the input pin			

   pinMode(SELECTOR_HIGH, OUTPUT);      // set the Switch pin mode  
   digitalWrite(SELECTOR_HIGH,HIGH);   // HighLevel used byu switch			
 
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("Twist around Blue Color for a while, then stop ");
   for (int i=0; i <= 10; i++){
		//  Blue	 Band
		R=0;
		G=0;
		B=255;	
		writeLEDS(R, G, B);   
		delay (50);
		//  Black	 Band
		R=0;
		G=0;
		B=0;	
		writeLEDS(R, G, B);   
		delay (50);		
   }
		   

}



//-------------------------------------------------------
void setup() 
//-------------------------------------------------------
{

  
  initHardware();
  myIp =setupWiFi();
  
  Serial.println("Access Point enabled, IP: ");  
  Serial.println(myIp);   
  //server.begin();
}

//-------------------------------------------------------
void loop_() 
//-------------------------------------------------------
{
wicoSetupWebServer();
}

//-------------------------------------------------------
void loop() 
//-------------------------------------------------------
{

	selector_previous=selector;
	selector = digitalRead(SELECTOR_I);   // read the input pin

	if (selector_previous!=selector)
	{
		Serial.println("First Transition Detected :-)");
		//Wait for a few second to see if another transition is detected
		delay (1000);
		selector_previous=selector;
		selector = digitalRead(SELECTOR_I);   // read the input pin
		if (selector_previous!=selector)
		{
			Serial.println("Second Transition Detected :-)");
	
			//Wait for a few second to see if another transition is detected
			delay (1000);
			selector_previous=selector;
			selector = digitalRead(SELECTOR_I);   // read the input pin		
			if (selector_previous!=selector)
			{
				Serial.println("Third Transition Detected => refresh profile from webserver");
				retrieveProfile();
				updateProfile();
			}
			else
			{			
				updateProfile();
			}			
		}
		else
		{
			startTimeKeeper();
		}
		
	}

}




//-------------------------------------------------------
// TIMEKEEPER FUNCTIONS
//-------------------------------------------------------

//-------------------------------------------------------
uint32_t retrieveProfile() {
//-------------------------------------------------------
  Serial.println("retrieveProfile from webserver:");
  //  Green	 Color
  writeLEDS(0,255,0);     
  wicoSetupWebServer();
  Serial.println("Twist around Green Color for a while, then stop ");
   for (int i=0; i <= 10; i++){
		//  Blue	 Band
		R=0;
		G=255;
		B=0;	
		writeLEDS(R, G, B);   
		delay (50);
		//  Black	 Band
		R=0;
		G=0;
		B=0;	
		writeLEDS(R, G, B);   
		delay (50);		
   }
   //Seems we are interested in dynamic profile since we have just updated the value => force to this mode.
   selectedProfile=kDynamicProfile;
}

//-------------------------------------------------------
uint32_t updateTimeKeeperColor() {
//-------------------------------------------------------


			switch (stepNumber) {
				case 0:
					if (nbOfSecondsElapsed>=time1)
					{
						Serial.println("Going to Step2");                   // print the network name (SSID);
						stepNumber++;
					}
	  		  
				  break;
				case 1:
					if (nbOfSecondsElapsed>=time2)
					{
						Serial.println("Going to Step3");                   // print the network name (SSID);
						stepNumber++;
					}  		  
				  break;
				case 2:
					if (nbOfSecondsElapsed>=time3)
					{
						Serial.println("Time's Up Man!");                   // print the network name (SSID);
						stepNumber++;
						abortCycle=1;
					}   		  
				  break;		  
				default: 
					Serial.println("WTF????"); 
			}
			/*
			Serial.print("Selected Color for stepNumber: ");
			Serial.println(stepNumber);
			Serial.println(colorsKeeper[stepNumber][0]);
			Serial.println(colorsKeeper[stepNumber][1]);
			Serial.println(colorsKeeper[stepNumber][2]);
			pixels.setBrightness(255);
			*/			
			writeLEDS(colorsKeeper[stepNumber][0],colorsKeeper[stepNumber][1],colorsKeeper[stepNumber][2]);
			//delay (100);	
			
			
			//Check If The User wants to abort cycle
				selector_previous=selector;
				selector = digitalRead(SELECTOR_I);   // read the input pin
				if (selector_previous!=selector)
				{
					Serial.println("Transition Detected, abort session");
					writeLEDS(0,0,0);
					delay (30);				
					abortCycle=1;
					
				}				


}

//-------------------------------------------------------
uint32_t startTimeKeeper() {
//-------------------------------------------------------



	stepNumber=0;
	nbOfSecondsElapsed=0;
	abortCycle=0;
	
	
	//Refresh time values
     time1=profilesKeeper[selectedProfile][0];
     time2=profilesKeeper[selectedProfile][1];
     time3=profilesKeeper[selectedProfile][2];
 
	
	Serial.println("Start TimeKeeper");
	Serial.println("Selected Color");
	Serial.println(colorsKeeper[stepNumber][0]);
	Serial.println(colorsKeeper[stepNumber][1]);
	Serial.println(colorsKeeper[stepNumber][2]);
	pixels.setBrightness(255);
	writeLEDS(colorsKeeper[stepNumber][0],colorsKeeper[stepNumber][1],colorsKeeper[stepNumber][2]);
	delay (100);
	unsigned int done=0;
	lastConnectionTime = millis();
	while (abortCycle==0)
	{
	

	
		//Turn the light on with a smooth breath effect
		for (int i=50; i<200; i++) 
		{
			if (abortCycle) break;
			pixels.setBrightness(i+55);
			pixels.show(); 
			delay(30); 
			if (millis() - lastConnectionTime > 1000) 
			{
				//1 second has elapsed;
				lastConnectionTime = millis();
				nbOfSecondsElapsed++;
				updateTimeKeeperColor();
			}
		}
		
		for (int i=200; i>50; i--) 
		{ 
			if (abortCycle) break;
			pixels.setBrightness(i+55);
			pixels.show(); 
			delay(30); 
			if (millis() - lastConnectionTime > 1000) 
			{
				//1 second has elapsed;
				lastConnectionTime = millis();
				nbOfSecondsElapsed++;				
				updateTimeKeeperColor();
				
			}
		}
		


		
	}
}


//-------------------------------------------------------
uint32_t updateProfile() {
//-------------------------------------------------------
unsigned int done=0;
Serial.println("Start updateProfile");

				
				
//Pulse for a while to show which profile is selected
	lastConnectionTime = millis();
	while (done==0)
	{
		for (int i=0; i<selectedProfile+1; i++) 
		{ 
			writeLEDS(0,0,255);pixels.show(); delay(200); 
			writeLEDS(0,0,0);pixels.show(); delay(200); 
		}
		delay(1000); 
		if (millis() - lastConnectionTime > 7000) {
		done=1;
		}
		//Check If The User wants to update profile
		selector_previous=selector;
		selector = digitalRead(SELECTOR_I);   // read the input pin
		if (selector_previous!=selector)
		{
			Serial.println("Transition Detected, update profile");
			lastConnectionTime = millis();
			switch (selectedProfile) {
				case 0:
					selectedProfile++;
	  		  
				  break;
				case 1:
					selectedProfile++;  		  
				  break;
				case 2:
					selectedProfile=0;  		  
				  break;		  
				default: 
					Serial.println("WTF????"); 
			}			
			
		}		
	}
	


		
	Serial.println("UpdateProfile completed, now start the timekeeper");
	startTimeKeeper();

}







 /**********************************************************/
 /****            NEOPIXEL FUNCTIONS                    ****/
 /**********************************************************/


//-------------------------------------------------------
void writeLEDS(byte R, byte G, byte B)//basic write colors to the neopixels with RGB values
//-------------------------------------------------------
{
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




//Theatre-style crawling lights with rainbow effect
//-------------------------------------------------------
void theaterChaseRainbow(uint8_t wait) {
//-------------------------------------------------------
  for (int j=0; j < 128; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+2*j) % 255));    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}



//-------------------------------------------------------
void rainbow(uint8_t wait) {
//-------------------------------------------------------
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//-------------------------------------------------------
uint32_t Wheel(byte WheelPos) {
//-------------------------------------------------------
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


//-------------------------------------------------------
uint32_t Breath() {
//-------------------------------------------------------

	  Serial.println("Start Breath");
	  unsigned int done=0;
	  lastConnectionTime = millis();
	while (done==0)
	{

		for (int i=50; i<200; i++) { pixels.setBrightness(i); writeLEDS(R,G,B);pixels.show(); delay(30); }
		for (int i=200; i>50; i--) { pixels.setBrightness(i); writeLEDS(R,G,B);pixels.show(); delay(30); }
		if (millis() - lastConnectionTime > postingInterval) {
		done=1;
		}		
	}
	Serial.println("Stop Breath");  
	pixels.setBrightness(128);
	writeLEDS(R,G,B);
	pixels.show();              
	delay(3);  
}






//-------------------------------------------------------
uint32_t HeartBeat() {
//-------------------------------------------------------

  Serial.println("Start HeartBeat");
  unsigned int done=0;
  lastConnectionTime = millis();

  while (done==0)
  {


	   int x = 3;
	   for (int ii = 1 ; ii <252 ; ii = ii = ii + x){
		 pixels.setBrightness(ii);
		 pixels.show();              
		 delay(5);
		}
		
		x = 3;
	   for (int ii = 252 ; ii > 3 ; ii = ii - x){
		 pixels.setBrightness(ii);
		 pixels.show();              
		 delay(3);
		 }
	   delay(10);
	   
	   x = 6;
	  for (int ii = 1 ; ii <255 ; ii = ii = ii + x){
		 pixels.setBrightness(ii);
		 pixels.show();              
		 delay(2);  
		 }
	   x = 6;
	   for (int ii = 255 ; ii > 1 ; ii = ii - x){
		 pixels.setBrightness(ii);
		 pixels.show();              
		 delay(3);
	   }
	  delay (50);  

	  	  
		if (millis() - lastConnectionTime > postingInterval) {
		done=1;
		}
  }
  
  
	Serial.println("Stop HeartBeat");  
	 pixels.setBrightness(255);
	 pixels.show();              
	 delay(3);  
}

