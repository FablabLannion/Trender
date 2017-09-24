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
 
#include "inc/Trender_defs.h"
#include "inc/TDR_NeoPixel.h"

// class TDR_NeoPixel : public TDR_Sensor_in  
TDR_NeoPixel::TDR_NeoPixel(){
	_no_leds=0; 
  _sig_pin=0; 
  _pstrip=NULL;
  _color=0;
  _ptk=NULL;
}
TDR_NeoPixel::TDR_NeoPixel(uint16_t n, uint8_t p){
	_no_leds=n;/* could be replaced by _pstrip->numPixels() */
	_sig_pin=p;
  _showRainbow=false;
  _color=0;
	_pstrip =  new Adafruit_NeoPixel(_no_leds, _sig_pin, NEO_GRB + NEO_KHZ800);
  _ptk    =  new Ticker();
}
TDR_NeoPixel::~TDR_NeoPixel(){
	_pstrip->~Adafruit_NeoPixel();
  _ptk->~Ticker();
}

int  TDR_NeoPixel::setNoLeds(uint16_t n){/* could be replaced by _pstrip->numPixels() */
	_no_leds=n;
	return TDR_SUCCESS;
}
uint16_t  TDR_NeoPixel::getNoLeds(){
	return _no_leds;/* could be replaced by _pstrip->numPixels() */
}

int TDR_NeoPixel::setSigPin(uint8_t p) {
	_sig_pin=p;
	return TDR_SUCCESS;
}
uint8_t TDR_NeoPixel::getSigPin() {
	return _sig_pin;
}

uint8_t TDR_NeoPixel::setup() {
	_pstrip->begin();
	_pstrip->show(); /* Initialize all pixels to 'off' */

  _raincur=0;
 // _ptk->attach(0.05,tkTDR_NeoPixelColor,this);

	return TDR_SUCCESS;
}

char* TDR_NeoPixel::get_type(){
	return _type;
}

Adafruit_NeoPixel* TDR_NeoPixel::getStrip() {
  return _pstrip;
}

/** set all pixel to color
 * @param col the color as 0xRRGGBB
 */
int TDR_NeoPixel::setColor (uint32_t col) {
  uint8_t i=0;
  Serial.print(__FUNCTION__);
  Serial.print(" :: 0x");
  Serial.println(col, HEX);
  //     Serial.println(pixels.getPixelColor(0),HEX);
  //     Serial.println(col,HEX);
  for (i = 0; i < _no_leds; i++) {
    _pstrip->setPixelColor(i, col);
  }
  _pstrip->setBrightness (255);
  _pstrip->show();

  return TDR_SUCCESS;
}

/** Go to designated color
 *
 * Fade from current color. Each component (R,G,B) are (inc|dec)remented
 * by one step up to the given color.
 *
 * @param color to go
 * @param wait delay (ms) between each step
 */
int TDR_NeoPixel::gotoColor (uint32_t color, uint8_t wait) {

   uint32_t cColor = _pstrip->getPixelColor(0);  /** current color */
   uint8_t rc = cColor >> 16;               /** current red */
   uint8_t gc = cColor >> 8 & 0xFF;         /** current green */
   uint8_t bc = cColor & 0xFF;              /** current blue */
   int8_t  ri=1,gi=1,bi=1;                  /** inc/dec for each component */

  Serial.print(__FUNCTION__);
  Serial.print(" :: 0x");
  Serial.print(color, HEX);
  Serial.print(", from :: 0x");
  Serial.println(cColor, HEX);

   if (color == cColor)
       return TDR_SUCCESS;

   // compute inc or dec for each component
   if ( (rc) > (color>>16) ) {
      ri = -1;
   }
   if ( (gc) > (color>>8&0xFF) ) {
      gi = -1;
   }
   if ( (bc) > (color&0xFF) ) {
      bi = -1;
   }

   // goto
   while (cColor != color) {
      if (rc != (color>>16))
         rc += ri;
      if (gc != (color>>8&0xFF))
         gc += gi;
      if (bc != (color&0xFF))
         bc += bi;
      cColor = _pstrip->Color (rc,gc,bc);
      setColor (cColor);
      delay(wait);
   }
   return TDR_SUCCESS;
}



/** Input a value 0 to 255 to get a color value.
* The colours are a transition r - g - b - back to r.
* @param WheelPos current whell position 0-255
*/
uint32_t TDR_NeoPixel::Wheel() {
  byte WheelPos=_raincur;
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return _pstrip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return _pstrip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return _pstrip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void TDR_NeoPixel::initModeRainbow() {
	_raincur=0;
}
uint32_t TDR_NeoPixel::modeRainbow() {
  Serial.println(__FUNCTION__);
	for (int i = 0; i < _no_leds; i++) {
      _pstrip->setPixelColor(i, Wheel());
    }
    _pstrip->show();
    
    _raincur=(_raincur+1)%255;
    Serial.print("_raincur=");
    Serial.println(_raincur);
	return TDR_SUCCESS;
}

/* TDR_NeoPixel functions for HTML WebServer */
void sendColorData (TDR_WebServer *w, TDR_NeoPixel *n) {
  String values ="";
  ESP8266WebServer* s=w->getServer();
  values += "rainbow|" +  (String) (n->_showRainbow ? "checked" : "") + "|chk\n";
  s->send ( 200, "text/plain", values); 
  Serial.println(__FUNCTION__);
}

void processColor (TDR_WebServer *w, TDR_NeoPixel* n) {
  ESP8266WebServer* s=w->getServer();
  if (s->args() > 0) {
    if (s->hasArg("rainbow")) {
      n->_showRainbow = true;
      n->_ptk->attach(0.05,tkTDR_NeoPixelColor,n);
    }
    else {
      n->_showRainbow = false;
      n->_ptk->detach();
      n->_color = (s->arg("cR").toInt()&0xFF) << 16 |
                  (s->arg("cG").toInt()&0xFF) <<  8 |
                  (s->arg("cB").toInt()&0xFF);
      Serial.println(n->_color, HEX);
      tkTDR_NeoPixelColor(n);
    }
  }
  s->send ( 200, "text/html", PAGE_Color);
  //debug
  Serial.println(__FUNCTION__);
}

void tkTDR_NeoPixelColor(TDR_NeoPixel* n) {
  uint8_t i;
  uint16_t nleds      =n->getNoLeds();
  Adafruit_NeoPixel* p=n->getStrip();

  if(p==NULL) return;

  if (!n->_showRainbow) {
    for (i = 0; i < nleds; i++) {
      p->setPixelColor(i, n->_color);
    }
  } else {
    for (i = 0; i < nleds; i++) {
      p->setPixelColor(i, n->Wheel());
    }
    n->_raincur = (n->_raincur+1) % 255;
  }
  p->show();
}

// end of file
