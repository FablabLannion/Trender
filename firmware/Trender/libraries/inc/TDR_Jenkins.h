#ifndef __TDR_JENKINS_H__
#define __TDR_JENKINS_H__

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
#include "TDR_ThingSpeak.h"
#include "TDR_NeoPixel.h"


class TDR_Jenkins : public TDR_Usage {
protected:
	char*           _type = "Jenkins";
	TDR_ThingSpeak* _pTS;
	TDR_NeoPixel*   _pstrip;

public:

public:
	TDR_Jenkins();
	TDR_Jenkins(TDR_NeoPixel* n, TDR_ThingSpeak* t);
	~TDR_Jenkins();

	char*           get_type();
	unsigned char   get_last_code();
	unsigned char   get_last_TDR_mngt();
	void            setStrip(TDR_NeoPixel* s);
	void            setThingSpeak(TDR_ThingSpeak* t);
	TDR_NeoPixel*   getStrip();
	TDR_ThingSpeak* getThingSpeak();
	uint8_t         setStripColor(uint8_t R, uint8_t G, uint8_t B);

	uint8_t color(uint8_t *,unsigned int);
	uint8_t color1(uint8_t *);
	uint8_t color2(uint8_t *);
	uint8_t color3(uint8_t *);

};

#endif