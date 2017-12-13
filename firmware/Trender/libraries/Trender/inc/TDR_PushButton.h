#ifndef __TDR_PUSH_BUTTON_H__
#define __TDR_PUSH_BUTTON_H__

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

#include "TDR_Activator.h"



class TDR_PushButton : public TDR_Activator { 
protected:
	char*     _type = "pushbutton";
    uint8_t   _pin;

public:
	TDR_PushButton();
	TDR_PushButton(uint8_t p);
	~TDR_PushButton();

    uint8_t setPin(uint8_t p);
	uint8_t setup();
    uint8_t getVal();

    uint8_t showError();
    uint8_t showOK();
    uint8_t showChangeMode();

    char*   get_type();
};



#endif
