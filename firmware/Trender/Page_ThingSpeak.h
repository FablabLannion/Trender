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

#ifndef PAGE_THINGPSEAK_H
#define PAGE_THINGPSEAK_H


//
//   The HTML PAGE
//
const char PAGE_ThingSpeak[]  = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script>
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>ThingSpeak Mode enabled, reset device to go back to Configuration Mode</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" > 
<tr><td align="right">ChannelID :</td><td><span id="x_channelid"></span></td></tr>
<tr><td colspan="2" align="center"><a href="javascript:GetThingSpeakState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetThingSpeakState()
{
  setValues("/admin/infothingspeak");
}

window.onload = function ()
{
  load("style.css","css", function()
  {
    load("microajax.js","js", function()
    {
        GetThingSpeakState();
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====" ;





//
// FILL WITH INFOMATION
//

void send_thingspeak_values_html ()
{

  String values ="";

  values += "x_channelid|" + (String)channelID +  "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
  
  thingSpeakMode=1;
  tkt.detach();
  
 
  

}

#endif
