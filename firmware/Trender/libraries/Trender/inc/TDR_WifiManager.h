#ifndef __TDR_WIFIMANAGER_H__
#define __TDR_WIFIMANAGER_H__

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

#include <FS.h>           //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>  //https://github.com/bblanchon/ArduinoJson

#include "Trender_defs.h"


class TDR_WifiManager {
protected:
	char              _ssid[13];
	uint8_t			  _mac[6];
	char			  _tsChannelId[16];
	WiFiManager*      _pwman; 
	char              _timeKeeperMode[2];

public:
	TDR_WifiManager();
	~TDR_WifiManager();

	int   setup();
	int   backToConfigure();

	char* getTsChannelId();
};


void saveConfigCallback ();

#endif