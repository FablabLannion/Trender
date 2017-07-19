#ifndef __TRENDER_H__
#define __TRENDER_H__

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

#include <Arduino.h>
#include <list>
// #include <String>

#include "inc/Trender_defs.h"

#include "inc/TDR_WebServer.h"
#include "inc/TDR_WebClient.h"
#include "inc/TDR_Sensor.h"
#include "inc/TDR_Usage.h"

class Trender {
	protected:
		TDR_RETURN_t        tdr_return; 
		TDR_USAGES_t        tdr_usages;
		unsigned char		_thingSpeakMode;	/** TDR_FALSE if no TS is not used, TDR_TRUE otherwise */
		TDR_WebServer*      _webserver;			/** instanciate the Trender with active Web server */
		TDR_WebClient*		_webclient;			/** instanciate the Trender as a Web client */
		std::list<TDR_Sensor*>	_sensors; 		/** list of the current supported sensors */
		std::list<TDR_Usage*>	_usages;		/** list of the current supported usages */
		uint8_t             _uv;                /** usages vector */
//		TDR_AccessPoint*	_ap;				/** TBD if needed */
	public:
		Trender();
		Trender(TDR_WebServer* ws);
		Trender(TDR_WebClient* wc);
		Trender(TDR_Sensor* s);
		Trender(TDR_Usage* u);
		Trender(TDR_USAGES_t);

		~Trender();

		uint8_t	setup();
		uint8_t run();

		int setWebServer(TDR_WebServer *ws);
		int setWebClient(TDR_WebClient *wc);
		TDR_WebServer*  getWebServer();
		TDR_WebClient*  getWebClient();

		int  loadWebPages();
		int  serveWebRequest();


		uint8_t	setupSensors();
		int     showSensorsType();
		TDR_Sensor& findFirstSensorOf(char* type);
		std::list<TDR_Sensor*>* findAllSensorsOf(char* type);
		std::list<TDR_Usage*>*  findAllUsagesOf(char* type);

		int addSensor(TDR_Sensor* s);
		int addUsage(TDR_Usage* u);

		void modeDemo();

		void err_msg(const char* f, const char* msg);
		void showNameVersion();
		void showAllSensorsOf(char *type);
};



#endif