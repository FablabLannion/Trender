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
 *  Modification done by Morgan Richomme 2017 for OPNFV trender
 *  Trender has been created by Fablab Lannion
 *  
 *  This example is in public domain.
 */
#include <Adafruit_NeoPixel.h>
 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Time.h>

#define PIN D2
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 7

const char* ssid = "Your SSID HERE";
const char* password = "YOUR WIFI PASSWORD HERE";

const char* host = "build.opnfv.org";
const int httpsPort = 443;

const int DELAY_BETWEEN_REQ = 300000;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "25 69 92 77 49 BD F8 E9 92 E1 32 70 5B 19 E0 9B 55 18 11 BB";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// ***********************************************************
// ***********************************************************
// Setup
// ***********************************************************
// ***********************************************************
void setup() {
  WiFi.persistent(false);
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  //delay(10);
  WiFi.begin(ssid, password);
  //delay(10);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
  digitalClockDisplay();
  int res = getJobStatus(client);
  // time between 2 requests depends on led sequence
  ledManagement(res);
  // delay(DELAY_BETWEEN_REQ);
}

// ***********************************************************
// ***********************************************************
// functions
// ***********************************************************
// ***********************************************************


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
  // TODO modify the URL to retrieve the JOB status you are interested in
  String url = "/ci/view/functest/job/functest-apex-apex-daily-danube-daily-danube/lastBuild/api/json";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  // Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      // Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  
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
