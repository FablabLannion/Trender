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

#include "inc/TDR_PushButton.h"


TDR_PushButton::TDR_PushButton() {
    _pin=0xff;
}

TDR_PushButton::TDR_PushButton(uint8_t p) {
    _pin=p;
}

TDR_PushButton::~TDR_PushButton() {

}

uint8_t TDR_PushButton::setPin(uint8_t p) {
    _pin=p;
}

uint8_t TDR_PushButton::setup() {
    pinMode(_pin, INPUT);
    return TDR_SUCCESS;
}

uint8_t TDR_PushButton::getVal() {
    uint8_t ret=0xff;
    Serial.print(__func__);
    if(_pin!=0xff) {
        ret=digitalRead(_pin);
        Serial.printf(" :: val=0x%02x\n", ret);
    }
    else {
        Serial.printf(" :: NO VALUE (0x%02x)\n", ret);
    }
    return ret;
}

uint8_t TDR_PushButton::showError() {
    return TDR_SUCCESS;
}
uint8_t TDR_PushButton::showOK() {
    return TDR_SUCCESS;
}
uint8_t TDR_PushButton::showChangeMode() {
    return TDR_SUCCESS;
}


char *TDR_PushButton::get_type(){
    return _type;
}

/* end of file */