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
 
#include "inc/TDR_Usg_TimeKeeperWithPushButton.h"


// class TDR_Usg_TimeKeeperWithPushButton : public TDR_Usage 
TDR_Usg_TimeKeeperWithPushButton::TDR_Usg_TimeKeeperWithPushButton(){
	_ptk       = new Ticker();
	_startTime = 0;
	_stopTime  = 0;
	_previousMode = STOPPED;
	_currentMode  = STOPPED;
    _pstrip    = NULL;
    _pbutton = NULL;

	// config init
	_config.dur = 1;
	_config.colors[0] = 0x00feff;
	_config.colors[1] = 0xfffe00;
	_config.colors[2] = 0xff0000;
	_config.per[0] = 50;
	_config.per[1] = 90;
	EEPROM.begin(512);
	readConfig();
}
TDR_Usg_TimeKeeperWithPushButton::TDR_Usg_TimeKeeperWithPushButton(TDR_NeoPixel* n, TDR_PushButton* b) {
    _ptk       = new Ticker();
    _startTime = 0;
    _stopTime  = 0;
    _previousMode = STOPPED;
    _currentMode  = STOPPED;
    _pstrip    = n;
    _pbutton  = b;

    // config init
    _config.dur = 1;
    _config.colors[0] = 0x00feff;
    _config.colors[1] = 0xfffe00;
    _config.colors[2] = 0xff0000;
    _config.per[0] = 50;
    _config.per[1] = 90;
    EEPROM.begin(512);
    readConfig();
}

TDR_Usg_TimeKeeperWithPushButton::~TDR_Usg_TimeKeeperWithPushButton(){
	_ptk->~Ticker();
}

/** start timer
 */
int  TDR_Usg_TimeKeeperWithPushButton::start (void) {
    _currentMode = STARTED;
    _startTime = millis();

    Serial.print("START ");
    Serial.println(_startTime);

    _pstrip->setColor(_config.colors[0]);
    delay(500);
    
    _pstrip->setColor(_config.colors[1]);
    delay(500);
    
    _pstrip->setColor(_config.colors[2]);
    delay(500);
    
    if(_ptk==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: _ptk should be instanced"); Serial.println("--> Create one Ticker"); }
    _ptk->attach(1, tkTDR_Usg_TimeKeeperWithPushButton, this);

    return TDR_SUCCESS;
} // start

/** stop main timer
 */
int  TDR_Usg_TimeKeeperWithPushButton::stop (void) {
    _pstrip->setColor(0);
    _currentMode = _previousMode = STOPPED;
    _ptk->detach();
    // if (_config.hb) {
    //     _ptk_hb.detach();
    // }
    Serial.println("STOP");
}//stop

/** read config from eeprom
 */
void TDR_Usg_TimeKeeperWithPushButton::readConfig (void) {
    int i = 0, j = 0;
    uint8_t c;

    // check magic marker
    c = EEPROM.read (i++);
    Serial.println(c,HEX);
    if (c != 'G') return;
    c = EEPROM.read (i++);
    Serial.println(c,HEX);
    if (c != 'A') return;
    c = EEPROM.read (i++);
    Serial.println(c,HEX);
    if (c != 'L') return;

    _config.dur = EEPROM.read (i++);
    for (j = 0; j < 3; j++) {
      _config.colors[j]  = ((uint32_t)EEPROM.read (i++)) << 16;
      _config.colors[j] |= ((uint32_t)EEPROM.read (i++)) << 8;
      _config.colors[j] |= ((uint32_t)EEPROM.read (i++));
    }
    for (j = 0; j < 2; j++) {
      _config.per[j] = EEPROM.read (i++);
    }
//    _config.hb = EEPROM.read (i++);
}// readConfig

void TDR_Usg_TimeKeeperWithPushButton::setStrip(TDR_NeoPixel* s) {
    _pstrip=s;
}

TDR_NeoPixel* TDR_Usg_TimeKeeperWithPushButton::getStrip() {
	return _pstrip;
}

char* TDR_Usg_TimeKeeperWithPushButton::get_type() {
    return _type;
}
uint8_t TDR_Usg_TimeKeeperWithPushButton::getButtonStatus() {
    return _pbutton->getVal();
}


//
// friend functions
//

/** write config to eeprom
 */
void writeConfig(TDR_Usg_TimeKeeperWithPushButton *t) {
    if(t==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_Usg_TimeKeeperWithPushButton should be instancied"); return; }
    int i = 0, j = 0;

    // write magic marker
    EEPROM.write(i++,'G');
    EEPROM.write(i++,'A');
    EEPROM.write(i++,'L');

    EEPROM.write(i++,t->_config.dur);

    for (j = 0; j < 3; j++) {
      EEPROM.write(i++,t->_config.colors[j]>>16);
      EEPROM.write(i++,(t->_config.colors[j]&0x00FF00)>>8);
      EEPROM.write(i++,(t->_config.colors[j]&0x0000FF));
    }
    for (j = 0; j < 2; j++) {
      EEPROM.write(i++,t->_config.per[j]);
    }
    EEPROM.commit();
}// writeConfig

void sendConfigData (TDR_WebServer* w,TDR_Usg_TimeKeeperWithPushButton *t) {
    if(w==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_WebServer should be instancied"); return; }
    if(t==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_Usg_TimeKeeperWithPushButton should be instancied"); return; }

	uint8_t i;
	char str[7];
	String values = "";

	ESP8266WebServer* s=w->getServer();
    if(s==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: ESP8266WebServer should be instancied"); return; }

	values += "dur|" +  (String) (t->_config.dur) + "|input\n";
	sprintf(str, "%06X", t->_config.colors[0]);
	values += "col1|#" + String(str) + "|input\n";
	sprintf(str, "%06X", t->_config.colors[1]);
	values += "col2|#" + String(str) + "|input\n";
	sprintf(str, "%06X", t->_config.colors[2]);
	values += "col3|#" + String(str) + "|input\n";
	values += "per1|" +  (String) (t->_config.per[0]) + "|input\n";
	values += "per2|" +  (String) (t->_config.per[1]) + "|input\n";
	//values += "hb|" +  (String) (t->config.hb ? "checked" : "") + "|chk\n";
	s->send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__);
}

void processConfig (TDR_WebServer* w,TDR_Usg_TimeKeeperWithPushButton *t) {
    if(w==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_WebServer should be instancied"); return; }
    if(t==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_Usg_TimeKeeperWithPushButton should be instancied"); return; }

	ESP8266WebServer* s=w->getServer();
    if(s==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: ESP8266WebServer should be instancied"); return; }

	if (s->args() > 0) {
		if (s->hasArg("dur")) {
		  t->_config.dur = s->arg("dur").toInt();
		}
		if (s->hasArg("col1")) {
		  t->_config.colors[0] = strtol(s->arg("col1").substring(1).c_str(), NULL, 16);
		}
		if (s->hasArg("col2")) {
		  t->_config.colors[1] = strtol(s->arg("col2").substring(1).c_str(), NULL, 16);
		}
		if (s->hasArg("col3")) {
		  t->_config.colors[2] = strtol(s->arg("col3").substring(1).c_str(), NULL, 16);
		}
		if (s->hasArg("per1")) {
		  t->_config.per[0] = s->arg("per1").toInt();
		}
		if (s->hasArg("per2")) {
		  t->_config.per[1] = s->arg("per2").toInt();
		}
		// if (s->hasArg("hb")) {
		//     t->_config.hb = 1;
		// } else {
		//     t->_config.hb = 0;
		// }

	}

	s->send ( 200, "text/html", PAGE_Config);
	Serial.println(__FUNCTION__);
	Serial.println(t->_config.colors[0], HEX);
	Serial.println(t->_config.colors[1], HEX);
	Serial.println(t->_config.colors[2], HEX);
}

/** ticker callback for trender main timer
 */
void tkTDR_Usg_TimeKeeperWithPushButton(TDR_Usg_TimeKeeperWithPushButton* n) {
    ///Serial.println(__FUNCTION__);
    if(n==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_Usg_TimeKeeperWithPushButton should be instancied"); return; }

    unsigned long now     = millis();
    unsigned long quarter_length = 0;
    uint8_t       quarter = 0;

    TDR_NeoPixel* s       = n->getStrip();
    if(s==NULL) { Serial.print(__FUNCTION__); Serial.println(": [ERROR] :: TDR_NeoPixel should be instancied"); return; }

    if(n->getButtonStatus() == HIGH) {
        n->stop();
        n->start();
        return;
    }

    // compute the mode
    if (now < (n->_startTime + (n->_config.dur*60000)*n->_config.per[0]/100)) {
        n->_previousMode = n->_currentMode;
        n->_currentMode = 1;
    } else
    if (now < (n->_startTime + (n->_config.dur*60000)*n->_config.per[1]/100 )) {
        n->_previousMode = n->_currentMode;
        n->_currentMode = 2;
    } else
    if (now < (n->_startTime + (n->_config.dur*60000))) {
        n->_previousMode = n->_currentMode;
        n->_currentMode = 3;
    } else {
        n->stop();
        return; // exit now, next computation (hb) will divide by 0 (quater_length will be == 0)
    }
    if (n->_previousMode != n->_currentMode && n->_currentMode>0 && n->_currentMode<=3) {
        s->setColor (n->_config.colors[n->_currentMode-1]);
        n->_previousMode = n->_currentMode;
    }
    //s->setColor (n->_config.colors[n->_currentMode]);
    /* if heartbeat is activated, compute in which quater we are
     * set heartbeat ticker acordingly
     */
	/*
	// quarter1: one pulse
	// quarter2: two pulse
	// quarter3: three pulse
	// quarter4: four pulse
    if (config.hb) {
        quater_length = ( END_TIME_MODE(currentMode) - START_TIME_MODE(currentMode) ) / 4;
        quater = (now - START_TIME_MODE(currentMode) ) / quater_length;

        Serial.print(currentMode);Serial.print(",");
        Serial.print(START_TIME_MODE(currentMode));Serial.print(",");
        Serial.print(END_TIME_MODE(currentMode) );Serial.print(",");
        Serial.print(quater_length);Serial.print(",");
        Serial.println(quater);
        tkb.attach( ( TKB_BASE_RATE / (currentMode+1) ) / (quater+1) , tkHeartbeat);
    }
	*/

/* add it later */
#if 0 
	// quarter1: no pulse
	// quarter2: no pulse
	// quarter3: one pulse
	// quarter4: three pulse
    if (config.hb) {
        quater_length = ( END_TIME_MODE(currentMode) - START_TIME_MODE(currentMode) ) / 4;
        quater = (now - START_TIME_MODE(currentMode) ) / quater_length;
        Serial.print(currentMode);Serial.print(",");
        Serial.print(START_TIME_MODE(currentMode));Serial.print(",");
        Serial.print(END_TIME_MODE(currentMode) );Serial.print(",");
        Serial.print(quater_length);Serial.print(",");
        Serial.println(quater);
		switch (quater) {
			case 2:
			  tkb.attach( ( TKB_BASE_RATE / (currentMode+1) ) / (quater-1) , tkHeartbeat);
			  break;
			case 3:
			  tkb.attach( ( TKB_BASE_RATE / (currentMode+1) ) / (quater) , tkHeartbeat);
			  break;
			default:
			  tkb.detach();
			break;
		}
    }
#endif 
} // tkTDR_Usg_TimeKeeperWithPushButton
// end of file