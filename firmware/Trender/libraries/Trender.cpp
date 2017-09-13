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

#include "trender.h"
#include "inc/TDR_NeoPixel.h"
#include "inc/TDR_TimeKeeper.h"

// class Trender
Trender::Trender() {
	_thingSpeakMode=TDR_FALSE;
	_webserver     =NULL;
	_webclient     =NULL;
	_devices       ;
	_usages        ;
	_uv            =0;
}

Trender::Trender(TDR_WebServer* ws)
	: Trender()
{
	_webserver=ws;
}

Trender::Trender(TDR_WebClient* wc)
	: Trender()
{
	_webclient=wc;
}

Trender::Trender(TDR_Device* s)
	: Trender()
{
	_devices.push_back(s);
}

Trender::Trender(TDR_Usage* u)
	: Trender()
{
	_usages.push_back(u);
}

Trender::Trender(TDR_USAGES_t u=TDR_NOUSAGE) 
{
	TDR_NeoPixel* astrip=NULL;
	switch(u) {
	case TDR_NOUSAGE:
		/*nothing*/
		_thingSpeakMode=TDR_FALSE;
		_webserver     =NULL;
		_webclient     =NULL;
		_devices       ;
		_usages        ;
		_uv            =0;
		err_msg(__func__,"warning: no usage selected !");
		break;

	case TDR_USG_TIMEKEEPER:
		Serial.println("Trender:: Timekeeper usage selected!");
		astrip = new TDR_NeoPixel(1,D2); 
		_thingSpeakMode   = TDR_FALSE;
		_webserver        = new TDR_WebServer();
		_webclient        = NULL;
		_devices.push_back(astrip);     
		_usages.push_back(new TDR_TimeKeeper(astrip));
		_uv               = _uv|(1<<TDR_USG_TIMEKEEPER);
		break;

	case TDR_USG_JENKINS:
		Serial.println("(not implemented yet)Trender:: Jenkins usage selected!");
		break;

	case TDR_USG_PAPERLESS:
		Serial.println("(not implemented yet)Trender:: Paperless usage selected!");
		break;

	default:
		err_msg(__func__,"ERROR: usage not recognized !!!");
	}
}

Trender::~Trender(){
	_devices.~list();
	_usages.~list();
}

uint8_t Trender::run() {

	if( (_uv&(1<<TDR_USG_TIMEKEEPER)) >0 ) {
		serveWebRequest();
	}

	return TDR_SUCCESS;
}

uint8_t Trender::setup() {
	if(_webserver!=NULL) {
		_webserver->setup();
		loadWebPages();
	}
	if(_devices.size()>0) {
		setupDevices();
	}
	return TDR_SUCCESS;
}
uint8_t Trender::setupDevices() {
	TDR_Device* s = NULL;
	for(std::list<TDR_Device*>::iterator it = _devices.begin(); it != _devices.end(); it++) {
		s = *it;
		s->setup();
	}
	return TDR_SUCCESS;
}

int     Trender::showDevicesType() {
	TDR_Device* s = NULL;
	for(std::list<TDR_Device*>::iterator it = _devices.begin(); it != _devices.end(); it++) {
		s = *it;
		Serial.println(s->get_type());
	}
	return TDR_SUCCESS;	
}

TDR_Device& Trender::findFirstDeviceOf(char* type) {
	std::list<TDR_Device*>::iterator it = _devices.begin();
	for(; 
		it != _devices.end() && (strcmp((*it)->get_type(),type)!=0); 
		it++) ;
	return (TDR_Device&) *(*it);
}
std::list<TDR_Device*>* Trender::findAllDevicesOf(char* type) {
	if(&_devices==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: _devices should be instantiated"); return NULL; }

	std::list<TDR_Device*> *ldevices=new std::list<TDR_Device*>();
	for(std::list<TDR_Device*>::iterator it = _devices.begin(); it != _devices.end() ; it++) {
		Serial.println((*it)->get_type());
		if(strcmp((*it)->get_type(),type)==0) {
			ldevices->push_back(*it);
		}
	}
	return ldevices;
}
std::list<TDR_Usage*>* Trender::findAllUsagesOf(char* type) {
	if(&_usages==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: _usages should be instantiated"); return NULL; }
	
	Serial.println(__FUNCTION__);
	std::list<TDR_Usage*> *lusages=new std::list<TDR_Usage*>();
	for(std::list<TDR_Usage*>::iterator it = _usages.begin(); it != _usages.end() ; it++) {
		if((*it)==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: (*it) null"); }
		else {
			Serial.print("current usage in the list: ");
			Serial.print((int)(*it), HEX);
			Serial.println((*it)->get_type());
			if(strcmp((*it)->get_type(),type)==0) {
				lusages->push_back(*it);
			}
		}
	}
	Serial.print("lusages size="); Serial.println(lusages->size());
	return lusages;
}

int Trender::setWebServer(TDR_WebServer *ws){
	_webserver=ws;
	return TDR_SUCCESS;
}

int Trender::setWebClient(TDR_WebClient *wc){
	_webclient=wc;
	return TDR_SUCCESS;
}

TDR_WebServer*  Trender::getWebServer() {
	return _webserver;
}
TDR_WebClient*  Trender::getWebClient() {
	return _webclient;
}


int  Trender::addDevice(TDR_Device* s){
	_devices.push_back(s);
	return TDR_SUCCESS;
}

int  Trender::addUsage(TDR_Usage* u){
	_usages.push_back(u);
	/* todo: add a switch case */
}

void Trender::modeDemo() {
	//debug
//	Serial.println(__FUNCTION__);

	std::list<TDR_Device*> *strips=findAllDevicesOf("neopixel");
	
	for(std::list<TDR_Device*>::iterator it = strips->begin(); it != strips->end(); it++) {
		TDR_NeoPixel* a_strip = (TDR_NeoPixel*)(*it);
		a_strip->modeRainbow();
	}
	strips->std::list<TDR_Device*>::~list();
}

void  Trender::showNameVersion(){
	Serial.print(HOSTNAME_PREFIX);
	Serial.print(" ");
	Serial.print(TDR_VERSION);
}

void Trender::err_msg(const char* f, const char* msg) {
	Serial.println();
	Serial.print(*f);
	Serial.print(" :: ");
	Serial.println(msg);
}

void Trender::showAllDevicesOf(char *type) {
	std::list<TDR_Device*>* l=findAllDevicesOf(type);
	for(std::list<TDR_Device*>::iterator it = l->begin(); it != l->end(); it++) {
		Serial.println((*it)->get_type());
	}
	l->std::list<TDR_Device*>::~list();
}

int  Trender::loadWebPages() {
	ESP8266WebServer*        pserver=_webserver->getServer();
	std::list<TDR_Device*>*  lneopix=(std::list<TDR_Device*>*) findAllDevicesOf("neopixel");
	std::list<TDR_Device*>::iterator it=lneopix->begin();
	TDR_NeoPixel*            pneopix=(TDR_NeoPixel*)(*it);
	std::list<TDR_Usage*>*   ltimekeeper=(std::list<TDR_Usage*>*) findAllUsagesOf("timekeeper");
	std::list<TDR_Usage*>::iterator itk=ltimekeeper->begin();
	TDR_TimeKeeper*          ptimekeeper=(TDR_TimeKeeper*)(*itk);

	Serial.println(__FUNCTION__);

	pserver->on ( "/", [pserver]() { Serial.println("/.html"); pserver->send ( 200, "text/html", PAGE_AdminMainPage );   }  );

  	pserver->on ( "/admin", [pserver]() { Serial.println("admin.html"); pserver->send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  	pserver->on ( "/admin/infovalues", [this]() { send_information_values_html(this->getWebServer()); } );
/*
  	pserver->on ( "/admin/infothingspeak", send_thingspeak_values_html);
*/  
 	pserver->on ( "/info", [pserver]() { Serial.println("info.html"); pserver->send ( 200, "text/html", PAGE_Information );   }  );
	pserver->on ( "/color", [this,pneopix]() { processColor(this->getWebServer(),pneopix); } );
	pserver->on ( "/color/values", [this,pneopix]() { sendColorData(this->getWebServer(),pneopix); } );
	if( (_uv&(1<<TDR_USG_TIMEKEEPER)) >0 ) {
 		pserver->on ( "/config", [this,ptimekeeper,pserver]() { processConfig(this->getWebServer(),ptimekeeper); /* pserver->send ( 200, "text/html", PAGE_Config); */ writeConfig(ptimekeeper);});
		pserver->on ( "/config/values", [this,ptimekeeper]() { sendConfigData(this->getWebServer(),ptimekeeper);});
	}
 /*
  	pserver->on ( "/thingspeak", []() { Serial.println("thingspeak.html"); pserver->send ( 200, "text/html", PAGE_ThingSpeak );   }  );  
  	pserver->on ( "/config/values", []() { Serial.println("Disable ThingSpeak timeout mode"); thingSpeakMode=0;tkt.detach();sendConfigData();});
*/
  	pserver->on ( "/style.css", [pserver]() { Serial.println("style.css"); pserver->send ( 200, "text/plain", PAGE_Style_css );  } );
  	pserver->on ( "/microajax.js", [pserver]() { Serial.println("microajax.js"); pserver->send ( 200, "text/plain", PAGE_microajax_js );  } );
	if( (_uv&(1<<TDR_USG_TIMEKEEPER)) >0 ) {
	  	pserver->on ( "/start", [pserver,ptimekeeper]() { Serial.println("start"); ptimekeeper->start(); pserver->send ( 200, "text/html", PAGE_AdminMainPage ); } );
  		pserver->on ( "/stop", [pserver,ptimekeeper]() { Serial.println("stop"); ptimekeeper->stop(); pserver->send ( 200, "text/html", PAGE_AdminMainPage ); } );
	}
  	unsigned int favicon_ico_len = 1406; // here due to multiple declarations issue. TODO: move it in a class
  	pserver->on ( "/favicon.ico", [pserver,favicon_ico_len]() { Serial.println("favicon"); pserver->send_P(200, "image/x-icon", favicon_ico, favicon_ico_len); } ); 

  	pserver->onNotFound ( [pserver]() { Serial.println("Page Not Found"); pserver->send ( 404, "text/html", "Page not Found" );   }  );

  	pserver->begin();

}

int   Trender::serveWebRequest() {
	if(_webserver!=NULL) {
		if(_webserver->serveWebRequest()!=TDR_SUCCESS) {
			return TDR_ERROR_2;
		}
	}
	else {
		return TDR_ERROR_1;
	}
	return TDR_SUCCESS;
}



// end of file