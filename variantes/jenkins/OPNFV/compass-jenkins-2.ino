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
 *  Adaptation pour jobs OPNFV by Morgan RIchomme, 2017
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Time.h>

const char* ssid = "Le_SSID_de_votre_point_d'accès";
const char* password = "Le_mot_de_passe_WIFI";

const char* host = "build.opnfv.org";
const int httpsPort = 443;

const int DELAY_BETWEEN_REQ = 300000; // Temps entre 2 requêtes

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "25 69 92 77 49 BD F8 E9 92 E1 32 70 5B 19 E0 9B 55 18 11 BB";

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

  /* Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  */
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
  ledManagement(res);
  delay(DELAY_BETWEEN_REQ);
}

// ***********************************************************
// ***********************************************************
// functions
// ***********************************************************
// ***********************************************************


/*
 * This function is used to translate Jenkisn into an error code
 * TODO: changer l'url en fonction du job que vous souhaitez visualiser
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
  String url = "/ci/view/functest/job/functest-compass-baremetal-daily-master/lastBuild/api/json";
  //String url = "/ci/view/functest/job/fuel-deploy-armband-baremetal-daily-master/lastBuild/api/json";
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
      break;
    case 0:
      Serial.println("Run green led sequence");
      break;
    case 1:
      Serial.println("Run yellow led sequence");
      break;
    default:
      Serial.println("Run random sequence");
    break;
  }
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
