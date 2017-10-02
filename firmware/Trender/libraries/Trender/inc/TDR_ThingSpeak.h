#ifndef __TDR_THINGSPEAK_H__
#define __TDR_THINGSPEAK_H__

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

#include "TDR_WebConnector.h"

// The type of data that we want to extract from the page
struct TS_UserData {
	char created_at[128]; 
	unsigned long entry_id;
	unsigned char code;
	unsigned int  color1; 
	unsigned int  color2; 
	unsigned int  color3;  
	unsigned char field5;   
	unsigned char field6;   
	unsigned char field7;
	unsigned char TDR_mngt;
};

#define  HTTP_TIMEOUT       20000  // max response time from server
#define  MAX_CONTENT_SIZE   512    // max size of the HTTP response


class TDR_ThingSpeak : public TDR_WebConnector {
protected:
	WiFiClient*    _client;
	char*          _thingSpeakServer;  // server's address
	unsigned char  _thingSpeakPort;
	char*          _thingSpeakChannelID;

	// resource1 & resource2 are constant values to be used to parse json file computed by thingspeal
	char*          _resource1;   // http resource screened 
	char*          _resource2; 

	unsigned char  _newSampleDetected=0;
	char*          _timestamp;

	unsigned char   _last_code;
	unsigned int    _last_color1;
	unsigned int    _last_color2;
	unsigned int    _last_color3;
	unsigned char   _last_TDR_mngt;

	unsigned int    _errors;

public:
	TDR_ThingSpeak();
	TDR_ThingSpeak(char* channelID);
	~TDR_ThingSpeak();

	char*   getChannelID();
	uint8_t setChannelID(char* ch);

	uint8_t interact();
	uint8_t sendRequest(char* channelID);
	bool    skipResponseHeaders();
	bool    skipThingSpeakHeader();
	uint8_t readResponseContent(char* content, size_t maxSize);
	uint8_t parseUserData(char *content,TS_UserData* userData);
	uint8_t printUserData(TS_UserData* userData);

	unsigned int  json_getColor(const char*);

	unsigned char get_last_code();
	uint8_t       getNumErrors();
	unsigned int  get_color1();
	unsigned int  get_color2();
	unsigned int  get_color3();
	unsigned char get_last_TDR_mngt();
};

#endif 