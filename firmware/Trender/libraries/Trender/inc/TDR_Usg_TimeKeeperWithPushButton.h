#ifndef __TDR_USG_TK_WITH_PUSH_BUTTON_H__
#define __TDR_USG_TK_WITH_PUSH_BUTTON_H__

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
 
#include <Ticker.h>
#include <EEPROM.h>

#include "TDR_Usage.h"
#include "TDR_WebServer.h"
//#include "TDR_Activator.h"
#include "TDR_PushButton.h"
#include "TDR_NeoPixel.h"


typedef struct t_config {
  uint8_t  dur;             /**< total duration (min) */
  uint32_t colors[3];       /**< colors to display */
  uint8_t  per[3];          /**< threasholds, last one is always 100 */
//  uint8_t  hb;              /**< heartbeat mode */
} T_CONFIG;

/** current/previous timer mode
 * will be 0 for 1st color, 1 then 2
 * can also be STOPPED
 */
#define STARTED 0
#define STOPPED 255
/** ending time (in millis()) of mode x
 */
#define END_TIME_MODE(x) ( _startTime + (_config.dur*60000) * _config.per[x] / 100 )
/** begining time (in millis()) of mode x
 */
#define START_TIME_MODE(x) ( (x==0)? _startTime : END_TIME_MODE(x-1) )

class TDR_Usg_TimeKeeperWithPushButton : public TDR_Usage {
protected:
	char*         _type = "timekeeper";
	Ticker*       _ptk;
//	Ticker*       _ptk_hb;
	TDR_NeoPixel* _pstrip;
    TDR_PushButton* _pbutton;

public:
	unsigned long _startTime;
	unsigned long _stopTime;
	T_CONFIG      _config;
	uint8_t       _previousMode;
	uint8_t       _currentMode;

public:
	TDR_Usg_TimeKeeperWithPushButton();
	TDR_Usg_TimeKeeperWithPushButton(TDR_NeoPixel* n,TDR_PushButton* b);
	~TDR_Usg_TimeKeeperWithPushButton();

	int  start();
	int  stop();

	char*         get_type();
	void          setStrip(TDR_NeoPixel* s);
	TDR_NeoPixel* getStrip();
    uint8_t       getButtonStatus();

	void readConfig();
};

void writeConfig(TDR_Usg_TimeKeeperWithPushButton *t);
void sendConfigData (TDR_WebServer* w,TDR_Usg_TimeKeeperWithPushButton *t);
void processConfig (TDR_WebServer* w,TDR_Usg_TimeKeeperWithPushButton *t);
void tkTDR_Usg_TimeKeeperWithPushButton(TDR_Usg_TimeKeeperWithPushButton* n);

#endif