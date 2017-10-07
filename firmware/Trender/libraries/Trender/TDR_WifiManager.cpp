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
	strcpy(_timeKeeperMode,"0");
}

TDR_WifiManager::~TDR_WifiManager(){
	if(_pwman!=NULL) delete _pwman;
}

int  TDR_WifiManager::setup() {
	char c;
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

					if(!json.containsKey("_tsChannelId"))
						strcpy(_tsChannelId,"0");
					else {
						const char* s=json["_tsChannelId"].asString();
						if(s==NULL) 
							strcpy(_tsChannelId,"0");
						else {
							strcpy(_tsChannelId, json["tsChannelId"]);
						}
					}
					if(!json.containsKey("timeKeeperMode"))
						strcpy(_timeKeeperMode,"0");
					else {
						const char* s=json["timeKeeperMode"].asString();
						if(s==NULL) 
							strcpy(_timeKeeperMode,"0");
						else {
							strcpy(_timeKeeperMode, json["timeKeeperMode"]);
						}
					}

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
	
	WiFiManagerParameter custom_text("<p>ThingSpeak Channel ID:");
	_pwman->addParameter(&custom_text);
/*                                            id/name   placeholder/prompt     default length */
	WiFiManagerParameter tsChannelIdParam = WiFiManagerParameter("channel","ThingSpeak Channel ID",_tsChannelId,16);
	_pwman->addParameter(&tsChannelIdParam);
	WiFiManagerParameter custom_textend("</p>");
	_pwman->addParameter(&custom_textend);

	WiFiManagerParameter custom_text2("<p>timeKeeper Mode (leave \"0\" to not enter timeKeeper mode)");
	_pwman->addParameter(&custom_text2);
	WiFiManagerParameter timeKeeperModeParam = WiFiManagerParameter("timeKeeperMode","switch to timeKeeperMode",_timeKeeperMode,16);
	_pwman->addParameter(&timeKeeperModeParam);
	WiFiManagerParameter custom_textend2("</p>");
	_pwman->addParameter(&custom_textend2);


	//set config save notify callback
	_pwman->setSaveConfigCallback(saveConfigCallback);

	WiFi.macAddress(_mac);
	snprintf (_ssid, 13, HOSTNAME_PREFIX"-%02X%02X", _mac[4], _mac[5]);

	if( ! _pwman->autoConnect(_ssid) ) {
		Serial.println(__FUNCTION__);
		Serial.print("_timeKeeperMode=");
		Serial.println(_timeKeeperMode);
		Serial.print(" ; timeKeeperModeParam=");
		Serial.println(timeKeeperModeParam.getValue());
		
		strncpy(&c,timeKeeperModeParam.getValue(),1);	
		Serial.print("c=");
		Serial.println(c);

		if(c!='0'){
			strcpy(_timeKeeperMode,"0");
			return TDR_ERROR_3;
		}

		Serial.println("failed to connect and hit timeout");
    	delay(3000);
    	//reset and try again, or maybe put it to deep sleep
    	ESP.reset();
		delay(5000);
	}

	//if you get here you have connected to the WiFi
	Serial.println("connected...yeey :)");

	Serial.print("timeKeeperModeParam=");

	strncpy(&c,timeKeeperModeParam.getValue(),1);	
	Serial.print("c=");
	Serial.println(c);
	if(c!='0'){
		strcpy(_timeKeeperMode,"0");
		return TDR_ERROR_3;
	}

	///Serial.print(tsChannelIdParam.getValue());
	//read updated parameters
	strcpy(_tsChannelId,tsChannelIdParam.getValue());
	///Serial.print(" ; ");
	///Serial.println(_tsChannelId);
	
	//save the custom parameters to FS
	if (shouldSaveConfig) {
		///Serial.println("saving config");
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.createObject();
		json["tsChannelId"] = _tsChannelId;
		json["timeKeeperMode"] = _timeKeeperMode;

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
	
	return TDR_SUCCESS;
}


int  TDR_WifiManager::backToConfigure() {
	_pwman->resetSettings();
	return TDR_SUCCESS;
}

char* TDR_WifiManager::getTsChannelId() {
	return _tsChannelId;
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// end of file