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

#ifndef PAGE_INFOMATION_H
#define PAGE_INFOMATION_H


//
//   The HTML PAGE
//
const char PAGE_Information[]  = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script>
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Network Information</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
  setValues("/admin/infovalues");
}

window.onload = function ()
{
  load("style.css","css", function()
  {
    load("microajax.js","js", function()
    {
        GetState();
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====" ;

String GetMacAddress()
{
  uint8_t mac[6];
    char macStr[18] = {0};
  WiFi.macAddress(mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return  String(macStr);
}


//
// FILL WITH INFOMATION
//

void send_information_values_html ()
{

  String values ="";

  values += "x_ssid|" + (String)mySSID +  "|div\n";
  values += "x_ip|" +  (String) WiFi.softAPIP()[0] + "." +  (String) WiFi.softAPIP()[1] + "." +  (String) WiFi.softAPIP()[2] + "." + (String) WiFi.softAPIP()[3] +  "|div\n";
  values += "x_mac|" + GetMacAddress() +  "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);

}


#endif
