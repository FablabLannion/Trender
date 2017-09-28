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
 
#include "inc/TDR_WifiManager.h"
bool shouldSaveConfig=false;
// class TDR_WifiManager 
TDR_WifiManager::TDR_WifiManager() {
	_pwman = NULL;
	strcpy(_tsChannelId,"");
}

TDR_WifiManager::~TDR_WifiManager(){
	if(_pwman!=NULL) delete _pwman;
}

int  TDR_WifiManager::setup() {
	//clean FS, for testing
	//SPIFFS.format();

	//read configuration from FS json
	Serial.println("mounting FS...");

	if (SPIFFS.begin()) {
		Serial.println("mounted file system");
		if (SPIFFS.exists("/config.json")) {
			//file exists, reading and loading
			Serial.println("reading config file");
			File configFile = SPIFFS.open("/config.json", "r");
			if (configFile) {
				Serial.println("opened config file");
				size_t size = configFile.size();
				// Allocate a buffer to store contents of the file.
				std::unique_ptr<char[]> buf(new char[size]);

				configFile.readBytes(buf.get(), size);
				DynamicJsonBuffer jsonBuffer;
				JsonObject& json = jsonBuffer.parseObject(buf.get());
				json.printTo(Serial);
				if (json.success()) {
					Serial.println("\nparsed json");

					strcpy(_tsChannelId, json["tsChannelId"]);

				} else {
					Serial.println("failed to load json config");
				}
			}
		}
	} else {
		Serial.println("failed to mount FS");
	}
	//end read
	if(_pwman==NULL) {
		_pwman = new WiFiManager();
	}

	WiFiManagerParameter custom_text("<p>ThingSpeak Channel ID:</p>");
	_pwman->addParameter(&custom_text);
/*                                            id/name   placeholder/prompt     default length */
	WiFiManagerParameter tsChannelIdParam = WiFiManagerParameter("channel","ThingSpeak Channel ID",_tsChannelId,16);
	_pwman->addParameter(&tsChannelIdParam);

	//set config save notify callback
	_pwman->setSaveConfigCallback(saveConfigCallback);
	
	// bool shouldSaveConfig=true;
	// //save the custom parameters to FS
	// if (shouldSaveConfig) {
	// 	Serial.println("saving config");
	// 	DynamicJsonBuffer jsonBuffer;
	// 	JsonObject& json = jsonBuffer.createObject();
	// 	json["tsChannelId"] = _tsChannelId;

	// 	File configFile = SPIFFS.open("/config.json", "w");
	// 	if (!configFile) {
	// 		Serial.println("failed to open config file for writing");
	// 	}

	// 	json.printTo(Serial);
	// 	json.printTo(configFile);
	// 	configFile.close();
	// 	//end save
	// }
	WiFi.macAddress(_mac);
	snprintf (_ssid, 13, "Trender-%02X%02X", _mac[4], _mac[5]);

	if( ! _pwman->autoConnect(_ssid) ) {
		Serial.println("failed to connect and hit timeout");
    	delay(3000);
    	//reset and try again, or maybe put it to deep sleep
    	ESP.reset();
		delay(5000);
	}

	//if you get here you have connected to the WiFi
	Serial.print("connected...yeey :)");	
	///Serial.print(tsChannelIdParam.getValue());
	//read updated parameters
	strcpy(_tsChannelId,tsChannelIdParam.getValue());
	///Serial.print(" ; ");
	///Serial.println(_tsChannelId);
	
	//bool shouldSaveConfig=true;
	//save the custom parameters to FS
	if (shouldSaveConfig) {
		///Serial.println("saving config");
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.createObject();
		json["tsChannelId"] = _tsChannelId;

		File configFile = SPIFFS.open("/config.json", "w");
		if (!configFile) {
			Serial.println("failed to open config file for writing");
		}

		///json.printTo(Serial);
		json.printTo(configFile);
		configFile.close();
		//end save
	}

    Serial.println("");
	Serial.println("local ip");
	Serial.println(WiFi.localIP());


/* -- old code 
	_pdns = new DNSServer();
	WiFi.macAddress(_mac);
	snprintf (_ssid, 13, "%s%02X%02X", baseSSID, _mac[4], _mac[5]);

	Serial.println("Configuring access point...");
	WiFi.hostname (HOSTNAME_PREFIX);
	WiFi.mode(WIFI_AP);
	WiFi.softAP(_ssid);
	Serial.println(WiFi.softAPIP());
*/
	/* load webpages in the Trender instance               */
	/* then call ::begin() (in the same Trender instance)  */
}


int  TDR_WifiManager::backToConfigure() {
	_pwman->resetSettings();
	return TDR_SUCCESS;
}

int  TDR_WifiManager::begin() {
	// // if DNSServer is started with "*" for domain name, it will reply with
 //  	// provided IP to all DNS request
 //  	_pdns->start ( DNS_PORT, "*", WiFi.softAPIP() );
 //  	// webserver
	// _pwman->begin();
  	Serial.println( "HTTP server started" );
  	return TDR_SUCCESS;
}

char* TDR_WifiManager::getTsChannelId() {
	return _tsChannelId;
}


// char* TDR_WifiManager::getSSID() {
// 	return _ssid;
// }
	
// int   TDR_WifiManager::serveWebRequest() {
// 	_pdns->processNextRequest();
// 	_pwman->handleClient();
// 	return TDR_SUCCESS;
// }

// ESP8266WebServer* TDR_WifiManager::getServer(){
// 	return _pwman;
// }

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


/* Admin functions for HTML WebServer */
// String GetMacAddress()
// {
// 	uint8_t mac[6];
// 	char macStr[18] = {0};
// 	WiFi.macAddress(mac);
// 	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
// 	return  String(macStr);
// }


//
// FILL WITH INFOMATION
//

// void send_information_values_html(TDR_WifiManager *w) {
// 	ESP8266WebServer* s=w->getServer();
// 	String values ="";

// 	values += "x_ssid|" + (String)w->getSSID() +  "|div\n";
// 	values += "x_ip|" +  (String) WiFi.softAPIP()[0] + "." +  (String) WiFi.softAPIP()[1] + "." +  (String) WiFi.softAPIP()[2] + "." + (String) WiFi.softAPIP()[3] +  "|div\n";
// 	values += "x_mac|" + GetMacAddress() +  "|div\n";
// 	s->send ( 200, "text/plain", values);
// 	Serial.println(__FUNCTION__);
// }

// end of file
