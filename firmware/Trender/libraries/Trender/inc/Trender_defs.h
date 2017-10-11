#ifndef __TRENDER_DEFS_H__
#define __TRENDER_DEFS_H__

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

#define  HOSTNAME_PREFIX	"Trender"
#define  TDR_VERSION 		"v2.0.5"

#define  TDR_FALSE			0
#define  TDR_TRUE			1
typedef enum {
 	TDR_SUCCESS=0,
 	TDR_ERROR_1,
 	TDR_ERROR_2,
 	TDR_ERROR_3
} TDR_RETURN_t;

typedef enum {
 	TDR_NOUSAGE=0,
 	TDR_USG_TIMEKEEPER=1,
 	TDR_USG_THINGSPEAK=2,
} TDR_USAGES_t;




 
#endif
