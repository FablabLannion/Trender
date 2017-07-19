#ifndef __TDR_NEOPIXEL_H__
#define __TDR_NEOPIXEL_H__

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
#include <Adafruit_NeoPixel.h>
#include "TDR_Sensor_out.h"
#include "TDR_WebServer.h"



class TDR_NeoPixel : public TDR_Sensor_out { 
protected:
	char*     _type = "neopixel";
	uint16_t  _no_leds; /** number of LEDs */
	uint8_t   _sig_pin; /** pin number     */
	Adafruit_NeoPixel* _pstrip; /** pointer to the strip of NeoPixel */
public:
	Ticker*   _ptk;
	boolean   _showRainbow;
	uint32_t  _color;
	uint8_t   _raincur;

public:
	TDR_NeoPixel();
	TDR_NeoPixel(uint16_t n, uint8_t p);
	~TDR_NeoPixel();

	uint8_t setup();
	char*   get_type();
	Adafruit_NeoPixel* getStrip();

	int setNoLeds(uint16_t n);
	uint16_t getNoLeds();

	int setSigPin(uint8_t p);
	uint8_t getSigPin();

	int setColor (uint32_t col) ;
	int gotoColor (uint32_t color, uint8_t wait);

	uint32_t Wheel();
	void     initModeRainbow();
	uint32_t modeRainbow();
};

/* TDR_NeoPixel functions for HTML WebServer */
void sendColorData (TDR_WebServer *w, TDR_NeoPixel *n);
void processColor (TDR_WebServer *w, TDR_NeoPixel* n);
void tkTDR_NeoPixelColor(TDR_NeoPixel* n);

#endif
