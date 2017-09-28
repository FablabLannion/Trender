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

#include "inc/TDR_ThingSpeak.h"

#include <ArduinoJson.h>

// class TDR_ThingSpeak 
TDR_ThingSpeak::TDR_ThingSpeak() {
	_client = new WiFiClient();
	_thingSpeakServer    = "api.thingspeak.com";
	_thingSpeakPort      = 80;
	_thingSpeakChannelID = 0;
	_resource1           = "/channels/";
	_resource2           = "/feeds/last.json";
	_newSampleDetected   = 0;
	_timestamp           = "dummydata";
	_last_code           = '0';
	_last_color1         =0xFF0000;
	_last_color2         =0xFF8000;
	_last_color3         =0x0080FF;
	_last_TDR_mngt       =0;
}

TDR_ThingSpeak::TDR_ThingSpeak(char* channelID)
	: TDR_ThingSpeak()
{
	_thingSpeakChannelID = channelID;
}	

TDR_ThingSpeak::~TDR_ThingSpeak(){

}

char* TDR_ThingSpeak::getChannelID() {
	return _thingSpeakChannelID;
}

uint8_t TDR_ThingSpeak::setChannelID(char* ch) {
	_thingSpeakChannelID=ch;
	return TDR_SUCCESS;
}

uint8_t TDR_ThingSpeak::interact() {
	/// Serial.print("enter ");
	/// Serial.println(__FUNCTION__);

	/// Serial.print("last_c1=");
	/// Serial.print(_last_color1,HEX);
	/// Serial.print("last_c2=");
	/// Serial.print(_last_color2,HEX);
	/// Serial.print("last_c3=");
	/// Serial.println(_last_color3,HEX);

	//------------------------------------ 
	// Interaction with ThingSpeak  
	//------------------------------------	
	// Retrieve latest value from ThingSpeak Database
	if (!_client->connect(_thingSpeakServer,_thingSpeakPort)) {
		Serial.println("_thingSpeakServer connection error");
		return TDR_ERROR_1;
	}

	Serial.print(__FUNCTION__);  
	Serial.print(" :: connected to thingspeak (channelID=");
	Serial.print(_thingSpeakChannelID);
	Serial.println(")");

	if (sendRequest(_thingSpeakChannelID)==TDR_SUCCESS && skipResponseHeaders() && skipThingSpeakHeader() ) {
		char response[MAX_CONTENT_SIZE];
		delay(2000);
		readResponseContent(response, sizeof(response));
		TS_UserData userData;
		if (parseUserData(response, &userData)) {
			printUserData(&userData);
		}
		
	}
	else {
		return TDR_ERROR_1;
	}

	return TDR_SUCCESS;
}

// Send the HTTP GET request to the server
// ---------------------------------------- 

uint8_t TDR_ThingSpeak::sendRequest(char* channelID) {
//  
	/// Serial.print("enter ");
	Serial.println(__FUNCTION__);

	if(channelID==0) {
		Serial.print(__FUNCTION__);
		Serial.println("_thingSpeakChannelID >0");
		return TDR_ERROR_1;
	}

	///Serial.println("Is there anybody out there?");
	//Serial.println(resource);

	_client->print("GET ");

	_client->print(_resource1);
	_client->print(channelID);
	_client->print(_resource2);
	_client->println(" HTTP/1.1");
	_client->print("Host: ");
	_client->println(_thingSpeakServer);
	_client->println("Connection: close");
	_client->println();

	return TDR_SUCCESS;
}

// Skip HTTP headers so that we are at the beginning of the response's body
// ----------------------------------------
bool  TDR_ThingSpeak::skipResponseHeaders() {
//
	/// Serial.print("enter ");
	/// Serial.println(__FUNCTION__);

	// HTTP headers end with an empty line
	char endOfHeaders[] = "\r\n\r\n";

	_client->setTimeout(HTTP_TIMEOUT);
	bool ok = _client->find(endOfHeaders);
	/// Serial.print(__FUNCTION__);
	/// Serial.print(" :: _client->find() returns ");
	/// Serial.println(ok);
	if (!ok) {
		Serial.println("No response or invalid response!");

		//Twist for a while to show we found something wrong
		for (int i=0; i <= 10; i++){
			//  Red	 Band
			// R=255;
			// G=0;
			// B=0;	
			// writeLEDS(R, G, B);   
			delay (50);
			//  White	 Band
			// R=0;
			// G=0;
			// B=0;	
			// writeLEDS(R, G, B);   
			delay (50);		
		}
		//  Red	 Band
		// R=255;
		// G=0;
		// B=0;	
		// writeLEDS(R, G, B);
	}
	return ok;
}

// Skip HTTP headers so that we are at the beginning of the response's body
//-------------------------------------------------------
bool TDR_ThingSpeak::skipThingSpeakHeader() {
//

	/// Serial.print("enter ");
	/// Serial.println(__FUNCTION__);

	// Flush first line to go to align to json body
	char endOfHeaders[] = "\r\n";

	_client->setTimeout(HTTP_TIMEOUT);
	bool ok = _client->find(endOfHeaders);
	/// Serial.print(__FUNCTION__);
	/// Serial.print(" :: _client->find() returns ");
	/// Serial.println(ok);

	if (!ok) {
		Serial.println(__FUNCTION__);
		Serial.println("ThingSpeak Header not found :-(");
		//Twist for a while to show we found something wrong
		for (int i=0; i <= 10; i++){
			//  Red	 Band
			// R=255;
			// G=0;
			// B=0;	
			// writeLEDS(R, G, B);   
			delay (50);
			//  White	 Band
  	// 		R=0;
			// G=0;
			// B=0;	
			// writeLEDS(R, G, B);   
			delay (50);		
		} 
		//  Red	 Band
		// R=255;
		// G=0;
		// B=0;	
		// writeLEDS(R, G, B);
	}

	return ok;
}

// Read the body of the response from the HTTP server
//-------------------------------------------------------
uint8_t TDR_ThingSpeak::readResponseContent(char* content, size_t maxSize) {
//
	/// Serial.print("enter ");
	/// Serial.println(__FUNCTION__);

	size_t length = _client->readBytes(content, maxSize);
	content[length] = '\0';
	Serial.print(__FUNCTION__);
	Serial.print(" :: length=");
	Serial.println(length);
	Serial.println(content);
	return TDR_SUCCESS;
}

// Parse the JSON from the input string and extract the interesting values
//-------------------------------------------------------
uint8_t TDR_ThingSpeak::parseUserData(char *content,TS_UserData* userData) {
//
	/// Serial.print("enter ");
	/// Serial.println(__FUNCTION__);

	// Compute optimal size of the JSON buffer according to what we need to parse.
	// This is only required if you use StaticJsonBuffer.
	const size_t BUFFER_SIZE = JSON_OBJECT_SIZE(10);     // the root object has 10 elements

	/// Serial.println(__FUNCTION__);
	/// Serial.println(content);
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
		return TDR_ERROR_1;
	}

	// Here were copy the strings we're interested in
	strcpy(userData->created_at, root["created_at"]);

	if(!root.containsKey("field8"))
		userData->TDR_mngt=0x40;
	else {
		const char* s=root["field8"].asString();
		if(s==NULL) 
			userData->TDR_mngt=0x20;
		else {
			userData->TDR_mngt=atoi(root["field8"]);
			_last_TDR_mngt=userData->TDR_mngt;
			if(userData->TDR_mngt==9) {
				return TDR_SUCCESS;
			}
		}
	}
	
	if(!root.containsKey("field1"))
		userData->code=0x80;
	else {
		const char* s=root["field1"].asString();
		if(s==NULL) 
			userData->code=0x10;
		else {
			userData->code=atoi(root["field1"]);
			_last_code=userData->code;
		}
	}
	
	if(!root.containsKey("field2"))
		userData->color1=0x81;
	else {
		const char* s=root["field2"].asString();
		if(s==NULL) 
			userData->color1=0x11;
		else {
			// Serial.print("********* NEW field2=");
			// Serial.println(root["field2"].asString());
			userData->color1=json_getColor(root["field2"].asString());
			// Serial.print("********* NEW userData->color1=0x");
			// Serial.println(userData->color1,HEX);
			_last_color1=userData->color1;
			Serial.print("********* NEW _last_color1=0x");
			Serial.println(_last_color1,HEX);
		}

	}

	if(!root.containsKey("field3"))
		userData->color2=0x82;
	else {
		const char* s=root["field3"].asString();
		if(s==NULL) 
			userData->color2=0x12;
		else {
			// Serial.print("********* NEW field3=");
			// Serial.println(root["field3"].asString());
			userData->color2=json_getColor(root["field3"].asString());
			// Serial.print("********* NEW userData->color2=0x");
			// Serial.println(userData->color2,HEX);
			_last_color2=userData->color2;
			Serial.print("********* NEW _last_color2=0x");
			Serial.println(_last_color2,HEX);
		}
	}

	if(!root.containsKey("field4"))
		userData->color3=0x83;
	else {
		const char* s=root["field4"].asString();
		if(s==NULL) 
			userData->color3=0x13;
		else {
			// Serial.print("********* NEW field4=");
			// Serial.println(root["field4"].asString());
			userData->color3=json_getColor(root["field4"].asString());
			// Serial.print("********* NEW userData->color3=0x");
			// Serial.println(userData->color3,HEX);
			_last_color3=userData->color3;
			Serial.print("********* NEW _last_color3=0x");
			Serial.println(_last_color3,HEX);
		}	}

	if(!root.containsKey("field5"))
		userData->field5=0x85;
	else {
		const char* s=root["field5"].asString();
		if(s==NULL) 
			userData->field5=0x15;
		else
			userData->field5=atoi(root["field5"]);
	}

	if(!root.containsKey("field6"))
		userData->field6=0x86;
	else {
		const char* s=root["field6"].asString();
		if(s==NULL) 
			userData->field6=0x16;
		else
			userData->field6=atoi(root["field6"]);
	}

	if(!root.containsKey("field7"))
		userData->field7=0x87;
	else {
		const char* s=root["field7"].asString();
		if(s==NULL) 
			userData->field7=0x17;
		else
			userData->field7=atoi(root["field7"]);
	}

	return TDR_SUCCESS;
}

// Print the data extracted from the JSON
//-------------------------------------------------------
void TDR_ThingSpeak::printUserData(TS_UserData* userData) {
//
	Serial.print("enter ");
	Serial.println(__FUNCTION__);

	Serial.print("old _timestamp: ");
	Serial.println(_timestamp);

	Serial.print("created_at = ");
	Serial.println(userData->created_at);
	Serial.print("Code = 0x");
	Serial.println(userData->code,HEX);
	// Serial.print("Field2 = ");
	// Serial.println(userData->field2);
	// Serial.print("Field3 = ");
	// Serial.println(userData->field3); 
	Serial.print("TDR_mngt = ");
	Serial.println(userData->TDR_mngt);


	//Check If this is a new sample
	if (strcmp(_timestamp,userData->created_at)  != 0) {
		Serial.println("Hey Dude! We have something new here, let's rock!");
		_newSampleDetected=1;
		// R=userData->field1;
		// G=userData->field2;
		// B=userData->field3;	
		strcpy(_timestamp,userData->created_at);
		// writeLEDS(R, G, B); //TODO : this should be done elsewhere, to control neopixel in a dedicated thread!
	}
	else { 
		Serial.println("go to bed...");
		_newSampleDetected=0;
	}
}

unsigned int TDR_ThingSpeak::json_getColor(const char* s) {
	char c;
	char r[3],g[3],b[3];
	int  i;
	uint8_t rv,gv,bv;

	Serial.print(__FUNCTION__);
	Serial.print("::s=");
	Serial.println(s);
	i=0;
	c=s[i++];
	if(c=='0') {
		c=s[i++];
		if(c=='x') {
			r[0]=s[i++];
			r[1]=s[i++];
			g[0]=s[i++];
			g[1]=s[i++];
			b[0]=s[i++];
			b[1]=s[i++];
		}
		else
			return 0;
	}
	else
		return 0;
	r[2]='\0';
	g[2]='\0';
	b[2]='\0';
	// rv=atoi(r);
	// gv=atoi(g);
	// bv=atoi(b);
	rv=strtoul(r,NULL,16);
	gv=strtoul(g,NULL,16);
	bv=strtoul(b,NULL,16);

	// Serial.print(__FUNCTION__);
	// Serial.print("::r=");
	// Serial.print(r);
	// Serial.print("::g=");
	// Serial.print(g);
	// Serial.print("::b=");
	// Serial.println(b);

	// Serial.print(__FUNCTION__);
	// Serial.print("::rv=");
	// Serial.print(rv);
	// Serial.print("::gv=");
	// Serial.print(gv);
	// Serial.print("::bv=");
	// Serial.println(bv);

	return ( (rv<<16) + (gv<<8) + bv );

}

unsigned char TDR_ThingSpeak::get_last_code() {
	// Serial.print("enter ");
	// Serial.println(__FUNCTION__);

	return _last_code;
}
unsigned int TDR_ThingSpeak::get_color1() {
	// Serial.print("enter ");
	// Serial.println(__FUNCTION__);

	return _last_color1;
}
unsigned int TDR_ThingSpeak::get_color2() {
	// Serial.print("enter ");
	// Serial.println(__FUNCTION__);

	return _last_color2;
}
unsigned int TDR_ThingSpeak::get_color3() {
	// Serial.print("enter ");
	// Serial.println(__FUNCTION__);

	return _last_color3;
}
unsigned char TDR_ThingSpeak::get_last_TDR_mngt() {
	// Serial.print("enter ");
	// Serial.println(__FUNCTION__);

	return _last_TDR_mngt;
}

// end of file
