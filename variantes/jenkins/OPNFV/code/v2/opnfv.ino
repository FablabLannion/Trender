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
 */
#include <Adafruit_NeoPixel.h>
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiClientSecure.h>
#include <Time.h>

#define PIN D2
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 7

#define HOSTNAME "opnfv"
#define baseSSID "OPNFV-"
#define VERSION "OPNFV Trender v1.0"
#define MYSSID_LEN 13
char mySSID[MYSSID_LEN];

char ssid[32];
char pwd[64];
char installer[64];
char opnfv_version[64];

const char* host = "build.opnfv.org";
const int httpsPort = 443;

const int DELAY_BETWEEN_REQ = 120000;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "25 69 92 77 49 BD F8 E9 92 E1 32 70 5B 19 E0 9B 55 18 11 BB";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

ESP8266WebServer server(80);

// ***********************************************************
// ***********************************************************
// Setup
// ***********************************************************
// ***********************************************************
void setup() {
  EEPROM.begin(512);

  // Init
  WiFi.persistent(false);
  Serial.begin(115200);
  Serial.println();
  Serial.printf ("\n\n%s\n", VERSION);

  // Wifi setup
  if (wicoReadWifiConfig (0, ssid, pwd, installer, opnfv_version) ==1) {
    Serial.println("SSID retrieved:");
    Serial.println(ssid);
    Serial.println("PWD retrieved:****************");
    Serial.println("Installer retrieved:");
    Serial.println(installer);
    Serial.println("OPNFV Version retrieved:");
    Serial.println(opnfv_version);  
  } else {
    Serial.println("Setup Wifi");
    setupWifi();
  }

  IPAddress my_ip = wicoSetupWifi(ssid, pwd);
  Serial.println(my_ip);
  delay(500);
  
  while ( my_ip  == IPAddress(0,0,0,0) ){
    Serial.println("IP address not valid");
    Serial.println("Clean EEprom");
    wicoResetWifiConfig(0);
    Serial.println("Setup local AP to get SSID");
    setupWifi();
    delay(500);
    Serial.println("Read EEprom values");
    wicoReadWifiConfig (0, ssid, pwd, installer, opnfv_version);
    my_ip = wicoSetupWifi(ssid, pwd);
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // init LEDs
  pixels.begin();
  
  }

// ***********************************************************
// ***********************************************************
// Loop
// ***********************************************************
// ***********************************************************
void loop() {
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

  // set a time stamp
  digitalClockDisplay();

  // Get CI status
  int res = getJobStatus(client);
  // time between 2 requests depends on led sequence
  ledManagement(res);
  
  delay(DELAY_BETWEEN_REQ);
}

// ***********************************************************
// ***********************************************************
// functions
// ***********************************************************
// ***********************************************************

// ------------------------------------------------
void setupWifi (void) {
  uint8_t r = 0;
  IPAddress myIP;
  char mySSID[MYSSID_LEN];
  uint8_t mac[6];

  blink (2, 100);

  // create ~uniq ssid
  WiFi.macAddress(mac);
  snprintf (mySSID, MYSSID_LEN, "%s%02X%02X", baseSSID, mac[4], mac[5]);

  // setup wifi
  do {
    r = wicoWifiConfig (0, mySSID, &myIP);
    Serial.println(myIP);
    if (r == 0 ) {
      digitalWrite (BUILTIN_LED, HIGH);
      // AP has been created, start web server
      Serial.write (mySSID);
      wicoSetupWebServer (myIP);
      digitalWrite (BUILTIN_LED, LOW);
    }
  } while (!r);

  blink (3, 100);
  // setup webserver
  WiFi.hostname (HOSTNAME);
  server.on ("/", handleRoot);
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println("HTTP server started");

}


void handleRoot(void) {
  char ssid[32];
  char pwd[64];
  char installer[64];
  char opnfv_version[64];

  // get current config
  memset (ssid,  0,  32);
  memset (pwd,   0,  64);
  memset (installer,   0,  64);
  memset (opnfv_version,   0,  64);
  wicoReadWifiConfig (0, ssid, pwd, installer, opnfv_version);

  String s = "<html><body><h1>OPNFV Trender</h1>";
  s += "<br/>SSID: ";  s += ssid;
  s += "<br/>Installer: "; s += installer;
  s += "<br/>Version: "; s += opnfv_version;
  s += "<p><form>Reset config <input type='checkbox' name='reset'><input type='submit' value='send'></form></p>\n";
  s += "</body></html>";
  if (server.hasArg("reset") ) {
    Serial.println("reset");
    wicoResetWifiConfig (0);
  }

  server.send ( 200, "text/html", s );
} // handleRoot()

void handleNotFound(void) {
  String message = "Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  server.send ( 404, "text/plain", message );
}


/** blink the builtin led
 *
 * usage:
 *  pinMode(BUILTIN_LED, OUTPUT);
 *  blink (5, 100);
 *
 * @param nb number of blinks
 * @param wait delay (ms) between blinks
 */
void blink (uint8_t nb, uint32_t wait) {
   // LED: LOW = on, HIGH = off
   for (int i = 0; i < nb; i++)
   {
      digitalWrite(BUILTIN_LED, LOW);
      delay(wait);
      digitalWrite(BUILTIN_LED, HIGH);
      delay(wait);
   }
}


/*
 * This function is used to translate Jenkisn into an error code
 * 
 * Status:
 * -2: UNKNOWN
 * -1: FAILURE
 * 0: SUCCESS
 * 1: PENDING
 * 
 */
int getJobStatus(WiFiClientSecure client){
  int result = -2;
  String url = "";
  if (installer == "apex") {
    url = "/ci/view/functest/job/functest-apex-apex-daily-";
    url += opnfv_version;
    url += "-daily-";
    url += opnfv_version;
    url += "/lastBuild/api/json"; 
  } else {
    url +=  "/ci/view/functest/job/functest-";
    url += installer;
    url += "-baremetal-daily-";
    url += opnfv_version;
    url += "/lastBuild/api/json";
  }

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
      //Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  //Serial.println(line);
  if (line.indexOf("SUCCESS") > 0) {
    Serial.println("Functest Compass last CI status: successfull!");
    result = 0;
  } else if (line.indexOf("FAILURE") > 0) {
    Serial.println("Functest Compass last CI status: failed");
    result = -1;
  } else {
    if (line.indexOf("duration: 0") > 0 ) {
      Serial.println("Functest last CI status: unknown state...");
    } else {
      Serial.println("Functest last CI status: build in progress....");
      result = 1;      
    }
  }
  
  return result;
}


/*
 * ledManagement
 * 
 * According to return code
 * Set led sequences
 * 
 */
void ledManagement(int res){
  switch (res) {
    case -1:
      Serial.println("Run red led sequence");
      Serial.println("Rosace rouge");
      clignotement(255,64,64,100);
      rosace(255,64,64,1000,20);
      k2000(255,64,64,100,20);
      clignotement(255,64,64,100);
      break;
    case 0:
      Serial.println("Run green led sequence");
       Serial.println("Clignotement vert");
        clignotement(0,100,0,100);
        delay(3000);
        rosace(0,100,0,1000,20);
        k2000(0,100,0,100,20);
        clignotement(0,100,0,100);
      break;
    case 1:
      Serial.println("Run yellow led sequence");
      Serial.println("Clignotement jaune");
      clignotement(255,255,0,100);
      k2000(255,255,0,100,20);
      rosace(255,255,0,1000,20);
      clignotement(255,255,0,100);
      break;
    default: 
      Serial.println("Run random sequence");
       Serial.println("Clignotement bleu");
       clignotement(0,255,255,100);
        rosace(0,100,0,1000,20);
        k2000(0,100,0,100,20);
       clignotement(0,255,255,100);
    break;
  }
   extinction();
   delay(10000);
}

/*
 * digital clock for the timestamp
 */
void digitalClockDisplay() {
 // digital clock display of the time
 Serial.print(hour());
 printDigits(minute());
 printDigits(second());
 Serial.print(" ");
 Serial.print(day());
 Serial.print(" ");
 Serial.print(month());
 Serial.print(" ");
 Serial.print(year());
 Serial.println();
}

/*
 * use to display the time
 */
void printDigits(int digits) {
 // utility function for digital clock display: prints preceding colon and leading 0
 Serial.print(":");
 if (digits < 10)
 Serial.print('0');
 Serial.print(digits);
}

void rosace(int couleur_R, int couleur_G, int couleur_B,int vitesse,int nb_tour){
  for(int k=0;k<nb_tour;k++){
    for(int j=0;j<NUMPIXELS;j++)
    {
      pixels.setPixelColor(j, pixels.Color(couleur_R, couleur_G, couleur_B));
      pixels.show();
      delay(vitesse);
    }
    extinction();
  }
}

void extinction(){
 for(int j=0;j<NUMPIXELS;j++)
  {
    pixels.setPixelColor(j, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

void allume_tout(int couleur_R, int couleur_G, int couleur_B){
  for(int j=0;j<NUMPIXELS;j++)
  {
    pixels.setPixelColor(j, pixels.Color(couleur_R, couleur_G, couleur_B));
    pixels.show();
  }

}
void k2000(int couleur_R, int couleur_G, int couleur_B,int vitesse, int nb_aller_retour){
  for(int k=0;k<nb_aller_retour;k++){
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.setPixelColor(1, pixels.Color(couleur_R, couleur_G, couleur_B));
      pixels.show();
      delay(vitesse);
     for(int j=2;j<NUMPIXELS;j++){
       pixels.setPixelColor(j-1, pixels.Color(0, 0, 0));
       pixels.setPixelColor(j, pixels.Color(couleur_R, couleur_G, couleur_B));
       pixels.show();  
       delay(vitesse);
     }
      pixels.setPixelColor(0, pixels.Color(couleur_R, couleur_G, couleur_B));
     for(int j=2;j<NUMPIXELS;j++){
       pixels.setPixelColor(9-j, pixels.Color(0, 0, 0));
       pixels.setPixelColor(8-j, pixels.Color(couleur_R, couleur_G, couleur_B));
       pixels.show();  
       delay(vitesse);
     }
  }
}

void clignotement(int couleur_R, int couleur_G, int couleur_B,int nb_clignotement){
  for(int j=0;j<nb_clignotement;j++)
  {
    allume_tout(couleur_R, couleur_G, couleur_B);
    delay(500);
    extinction();
    delay(500);
  }
}
