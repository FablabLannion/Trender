 /**
 *  This file is part of BBC Notif.
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
 *  Copyright 2017 Jérôme Labidurie jerome@labidurie.fr
 */

 /** Usage :
  * You should rename this file as credential.h and configure following
  * values with your own account informations.
  *
  * You need Base64 encoded email address and password of gmail account
  * which will be used to send emails.
  * You can use base64encode.org for encoding or shell command :
  *
  * $ echo example | base64
  * ZXhhbXBsZQ==
  *
  */
#ifndef __CREDENTIAL_H__
#define __CREDENTIAL_H__

#define CRED_EMAILBASE64_LOGIN "ZXhhbXBsZQ=="
#define CRED_EMAILBASE64_PASSWORD "UGFzc3dvcmQh"
#define CRED_FROM "example@gmail.com"

#endif // __CREDENTIAL_H__
