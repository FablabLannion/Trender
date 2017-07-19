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
 
#include "inc/TDR_WebServer.h"

// class TDR_WebServer 
TDR_WebServer::TDR_WebServer() {
	_pserver = new ESP8266WebServer(80); 
}
TDR_WebServer::TDR_WebServer(int port) {
	_pserver = new ESP8266WebServer(port); 
}

TDR_WebServer::~TDR_WebServer(){

}

int  TDR_WebServer::setup() {
	_pdns = new DNSServer();
	WiFi.macAddress(_mac);
	snprintf (_ssid, 13, "%s%02X%02X", baseSSID, _mac[4], _mac[5]);

	Serial.println("Configuring access point...");
	WiFi.hostname (HOSTNAME_PREFIX);
	WiFi.mode(WIFI_AP);
	WiFi.softAP(_ssid);
	Serial.println(WiFi.softAPIP());

	/* load webpages in the Trender instance               */
	/* then call ::begin() (in the same Trender instance)  */
}

int  TDR_WebServer::begin() {
	// if DNSServer is started with "*" for domain name, it will reply with
  	// provided IP to all DNS request
  	_pdns->start ( DNS_PORT, "*", WiFi.softAPIP() );
  	// webserver
	_pserver->begin();
  	Serial.println( "HTTP server started" );
  	return TDR_SUCCESS;
}

char* TDR_WebServer::getSSID() {
	return _ssid;
}
	
int   TDR_WebServer::serveWebRequest() {
	_pdns->processNextRequest();
	_pserver->handleClient();
	return TDR_SUCCESS;
}

ESP8266WebServer* TDR_WebServer::getServer(){
	return _pserver;
}


/* Admin functions for HTML WebServer */
String GetMacAddress()
{
	uint8_t mac[6];
	char macStr[18] = {0};
	WiFi.macAddress(mac);
	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
	return  String(macStr);
}


//
// FILL WITH INFOMATION
//

void send_information_values_html(TDR_WebServer *w) {
	ESP8266WebServer* s=w->getServer();
	String values ="";

	values += "x_ssid|" + (String)w->getSSID() +  "|div\n";
	values += "x_ip|" +  (String) WiFi.softAPIP()[0] + "." +  (String) WiFi.softAPIP()[1] + "." +  (String) WiFi.softAPIP()[2] + "." + (String) WiFi.softAPIP()[3] +  "|div\n";
	values += "x_mac|" + GetMacAddress() +  "|div\n";
	s->send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__);
}

// end of file
