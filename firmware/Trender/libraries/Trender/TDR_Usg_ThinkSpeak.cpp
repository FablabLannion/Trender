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
 
#include "inc/TDR_Usg_ThingSpeak.h"

// class TDR_Usg_ThingSpeak : public TDR_Usage 
TDR_Usg_ThingSpeak::TDR_Usg_ThingSpeak(){
	
    _pstrip    = NULL;

}
TDR_Usg_ThingSpeak::TDR_Usg_ThingSpeak(TDR_NeoPixel* n, TDR_ThingSpeak* t) {
    
    _pstrip    = n;
    _pTS       = t;
}

TDR_Usg_ThingSpeak::~TDR_Usg_ThingSpeak(){
	delete _pTS;
}

void TDR_Usg_ThingSpeak::setStrip(TDR_NeoPixel* s) {
    _pstrip=s;
}
void TDR_Usg_ThingSpeak::setThingSpeak(TDR_ThingSpeak* t) {
    _pTS=t;
}

TDR_NeoPixel* TDR_Usg_ThingSpeak::getStrip() {
	return _pstrip;
}
TDR_ThingSpeak* TDR_Usg_ThingSpeak::getThingSpeak() {
    return _pTS;
}

char* TDR_Usg_ThingSpeak::get_type() {
    return _type;
}

unsigned char TDR_Usg_ThingSpeak::get_last_code() {
    Serial.print(__FUNCTION__);
    Serial.print(" code=");
    Serial.println(_pTS->get_last_code());
    return _pTS->get_last_code();
}
unsigned char TDR_Usg_ThingSpeak::get_last_TDR_mngt() {
    Serial.print(__FUNCTION__);
    Serial.print(" TDR_mngt=");
    Serial.println(_pTS->get_last_TDR_mngt());
    return _pTS->get_last_TDR_mngt();
}

uint8_t TDR_Usg_ThingSpeak::getNumErrors() {
    return _pTS->getNumErrors();
}

uint8_t TDR_Usg_ThingSpeak::color(uint8_t *a,unsigned int c) {
    a[0] = (c>>16) & 0xFF;
    a[1] = (c>>8)  & 0xFF;
    a[2] =  c      & 0xFF;
    /// Serial.print("TDR_Usg_ThingSpeak::");
    /// Serial.print(__FUNCTION__);
    /// Serial.print(" : c=0x");
    /// Serial.print(c,HEX);
    /// Serial.print(" : [0]=0x");
    /// Serial.print(a[0],HEX);
    /// Serial.print(" : [1]=0x");
    /// Serial.print(a[1],HEX);
    /// Serial.print(" : [2]=0x");
    /// Serial.println(a[2],HEX);
    return TDR_SUCCESS;
}
uint8_t TDR_Usg_ThingSpeak::color1(uint8_t *a) {
    return color(a, _pTS->get_color1());
}
uint8_t TDR_Usg_ThingSpeak::color2(uint8_t *a) {
    return color(a, _pTS->get_color2());  
}
uint8_t TDR_Usg_ThingSpeak::color3(uint8_t *a) {
    return color(a, _pTS->get_color3());
}


uint8_t  TDR_Usg_ThingSpeak::setStripColor(uint8_t R, uint8_t G, uint8_t B) {
    Serial.print(__FUNCTION__);
    Serial.print(" R=0x");
    Serial.print(R,HEX);
    Serial.print(", G=0x");
    Serial.print(G,HEX);
    Serial.print(", B=0x");
    Serial.print(B,HEX);
    
    uint32_t c = (R<<16) + (G<<8) + B;
    Serial.print(", => c=0x");
    Serial.println(c,HEX);

    _pstrip->gotoColor(c,50);

    return TDR_SUCCESS;
}



// end of file