/**
 *  This file is part of BBC Notif.
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
 *  Copyright 2017 Jérôme Labidurie jerome@labidurie.fr
 */

/** wifiConfig
 *
 *  "library" to handle wifi connection/configuration via web server
 *  at the start of a esp8266.
 *
 *  basic usage :
 *  @code
 *  uint8_t r = 0;
 *  IPAddress myIp;
 *  EEPROM.begin(512);
 *  Serial.begin (115200);
 *
 *  do {
 *    r = wicoWifiConfig (0, "ESPconfig", &myIp);
 *    Serial.println(myIp);
 *    if (r == 0 ) {
 *       // AP has been created, start web server
 *       wicoSetupWebServer ();
 *    }
 *  } while (!r);
 *  @endcode
 *  What this code is doing :
 *
 *  Read wifi SSID/password from eeprom (at given address)
 *  Tries to connect to this wifi network
 *  If connection fail,
 *    start in Access Point mode
 *    start a webserver with scanned network and input for password
 *    when credential are entered, store them and connects to the said wifi network
 *
 *  @note if you want to reset the credentials stored in eeprom, you can use wicoResetWifiConfig()
 *  @code
 *  // html code embeded : <form>Reset wifi <input type='checkbox' name='reset'><input type='submit' value='send'></form>
 *  if (server.hasArg("reset") ) {
 *    Serial.println("reset");
 *    wicoResetWifiConfig (0);
 *  }
 *  @endcode
 *
 *  @todo: better webserver management (ie: global wicoServer(80)) ??
 *  @todo create a lib ?
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
ESP8266WebServer wicoServer(80);
DNSServer dnsServer;

#define NB_TRY 10 /**< duration of wifi connection trial before aborting (seconds) */
#define DNS_PORT 53 /**< DNS port */

int  wicoConfigAddr = 0; /**< address in eeprom to store wifi config */

#define SCAN_NETW 10  /**< max number of stored scan networks */
char wicoNetwSsid[SCAN_NETW][32]; /**< scanned wifi networks ssids (for AP mode) */
uint8_t wicoNetwSsidLen = 0; /**< number of really stored scan networks */
uint8_t wicoIsConfigSet = 0; /**< is wifi configuration has been set through webserver ? */

/** read wifi config stored in eeprom.
 *
 *  There must be at least 35+64+100 bytes after given configAddr.
 *  eeprom must have been initialised : EEPROM.begin(size);
 *
 * @param[in] configAddr eeprom address for begining of configuration
 * @param[out] ssid read ssid : 32 Bytes
 * @param[out] pwd read wifi password : 64 Bytes
 * @param[out] installer : 64 Bytes
 * @param[out] version: 64 Bytes
 * @return 1 if success, 0 if failure
 * @BUG: fin de chaines
 *
 */
int wicoReadWifiConfig (int configAddr, char* ssid, char* pwd, char* installer, char* opnfv_version) {
  int i = 0;

  for (i = 0; i < 32; i++) {
      ssid[i] = char(EEPROM.read(i+configAddr));
  }
  for (i=0; i < 64; i++) {
      pwd[i] = char(EEPROM.read(i+configAddr+32));
  }
  for (i=0; i < 64; i++) {
      installer[i] = char(EEPROM.read(i+configAddr+32+64));
  }
  for (i=0; i < 64; i++) {
      opnfv_version[i] = char(EEPROM.read(i+configAddr+32+64+64));
  }  
  return 1;
}


/** write wifi config to eeprom.
 *
 *  There must be at least 32+64+100 bytes after given configAddr.
 *  eeprom must have been initialised : EEPROM.begin(size);
 *  commit() is done here
 *
 *  @param[in] configAddr eeprom address for begining of configuration
 *  @param[in] ssid read ssid : 32 Bytes
 *  @param[in] pwd read wifi password : 64 Bytes
 *  @param[in] email read email : 100 Bytes
 *  @return 1 if success, 0 if failure
 */
int wicoWriteWifiConfig (int configAddr, const char ssid[32], const char pwd[64], const char installer[64], const char opnfv_version[64]) {
  int i = 0;

  for (i = 0; i < 32; i++)
    {
      EEPROM.write(i+configAddr,ssid[i]);
    }
  for (i=0; i < 64; i++)
    {
      EEPROM.write(i+configAddr+32,pwd[i]);
    }
  for (i=0; i < 64; i++)
    {
      EEPROM.write(i+configAddr+32+64,installer[i]);
    }
  for (i=0; i < 64; i++)
    {
      EEPROM.write(i+configAddr+32+64+64,opnfv_version[i]);
    }
    EEPROM.commit();
    return 1;
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
  char d2[32];
  memset (d2, 0, 32);
  return wicoWriteWifiConfig (configAddr, d2, d, d, d);
}

/** handle / URI for AP webserver
 */
void wicoHandleRoot (void) {
  String s = "<html><body><h1>OPNFV CI Config</h1>";

  // arguments management
  if (wicoServer.hasArg("reset") ) {
    Serial.println("reset");
    wicoResetWifiConfig (wicoConfigAddr);
  } else if (wicoServer.hasArg("ssid")) {
    Serial.print("save:");
    Serial.println(wicoServer.arg("ssid").c_str());
    Serial.print("save:");
    Serial.println(wicoServer.arg("pwd").c_str());
    Serial.print("save:");
    Serial.println(wicoServer.arg("installer").c_str());
    Serial.print("save:");
    Serial.println(wicoServer.arg("version").c_str());
    
    wicoWriteWifiConfig (wicoConfigAddr, wicoServer.arg("ssid").c_str(), wicoServer.arg("pwd").c_str(), wicoServer.arg("installer").c_str(), wicoServer.arg("opnfv_version").c_str());
    Serial.print("Config save in EEProm");
    wicoIsConfigSet = 1;
  }

  // construct <form>
  s += "<p><form>SSID: ";
  if ( wicoNetwSsidLen != 0) {
    // some netw were found
    s += "<select name='ssid'>";
     for (int i=0; i<wicoNetwSsidLen; i++) {
      Serial.print(i);
      Serial.println(wicoNetwSsid[i]);
        s += "<option>";
        s += wicoNetwSsid[i];
        s += "</option>";
     }
     s += "</select>";
  } else {
    s += "<input type=text name=ssid>";
  }
  s += "<br>Password: <input type='text' name='pwd'><br/>";
  s += "<br>Installer: <select name='installer'><br/><option>apex<br/><option>compass<br/><option>fuel<br/><option>joid<br/></select><br/>";
  s += "<br>Version: <select name='opnfv_version'><br/><option>danube<br/><option>master<br/></select><br/>";
  s += "<input type=checkbox name=reset>Reset<br/><input type='submit' value='send'></form></p></body></html>\n";

  wicoServer.send ( 200, "text/html", s );
}

/** start web server, DNSServer for CaptivePortal and wait for wifi configuration.
 * @param myIP current AP ip
 *  @return only when wifi configuration has been set
 */
void wicoSetupWebServer (IPAddress myIP) {
  wicoIsConfigSet = 0;
  wicoServer.on("/", wicoHandleRoot);
  wicoServer.begin();
  Serial.println("HTTP server started");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", myIP);

  while (!wicoIsConfigSet) {
    dnsServer.processNextRequest();
    wicoServer.handleClient();
  }
  wicoServer.stop();
}

/** setup open access point for wifi config
 *  @param[in] ssid ssid of the Access Point to create
 *  @return assigned ip address
 */
IPAddress wicoSetupAP(char* ssid) {
  int i;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(500);
  Serial.println("Scanning networks...");
  wicoNetwSsidLen  =  WiFi.scanNetworks();
  int n = WiFi.scanNetworks();
  if (n != 0) {
    // some networks were found, store them
    for (i=0; i<n && i<SCAN_NETW; i++) {
      strncpy ( wicoNetwSsid[i], WiFi.SSID(i).c_str(), 32);
    }
    wicoNetwSsidLen = i;
  }
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  return WiFi.softAPIP();
}

/** connect to an existing wifi network
 *  @param[in] ssid the ssid of the network
 *  @param[in] pwd the password of the network
 *  @return assigned ip address
 */
IPAddress wicoSetupWifi(char* ssid, char* pwd) {
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
  }
  return IPAddress (0,0,0,0);
}

/** configure wifi.
 *  read recorded informations from eeprom
 *  try to connect
 *  if failure, start AccessPoint
 *  @param(in] configAddr config data address
 *  @param[in] apSsid AP ssid name to be created (if needed)
 *  @param[out] myIp affected (wifi or AP) IP address
 *  @return 1 if wifi connection is ok, 0 is AP has been started
 */
int wicoWifiConfig (int configAddr, char* apSsid, IPAddress* myIp) {
  char ssid[32];
  char pwd[64];
  char installer[64];
  char opnfv_version[64];

  wicoConfigAddr = configAddr;

  // read config
  wicoReadWifiConfig (configAddr, ssid, pwd, installer, opnfv_version);
  Serial.print(ssid);
  Serial.print("/");
  Serial.print(pwd);
  Serial.println("/");
  Serial.print(installer);
  Serial.println("/");
  Serial.print(opnfv_version);
  Serial.println("/");
  *myIp = wicoSetupWifi (ssid, pwd);
  if (*myIp != IPAddress(0,0,0,0)) {
    // success
    return 1;
  }
  // connecting failed, start AP
  *myIp = wicoSetupAP(apSsid);

  return 0;
}

