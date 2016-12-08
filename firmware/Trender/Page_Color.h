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


const char PAGE_Color[] = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script>
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Color</strong>
<hr>
<form action="" method="post">
Rainbow: <input type="checkbox" id="rainbow" name="rainbow"><br/>

Red:   <input type="range" name="cR" min="0" max="255" /><br/>
Green: <input type="range" name="cG" min="0" max="255" /><br/>
Blue:  <input type="range" name="cB" min="0" max="255" /><br/>

<input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Set">
</form>
<script>
window.onload = function ()
{
  load("style.css","css", function()
  {
    load("microajax.js","js", function()
    {
        setValues("/color/values");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";

void sendColorData () {
  String values ="";
  values += "rainbow|" +  (String) (showRainbow ? "checked" : "") + "|chk\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
}

void processColor ()
{
  if (server.args() > 0) {
    if (server.hasArg("rainbow")) {
      showRainbow = true;
    }
    else {
      showRainbow = false;
      color = (server.arg("cR").toInt()&0xFF) << 16 |
              (server.arg("cG").toInt()&0xFF) <<  8 |
              (server.arg("cB").toInt()&0xFF);
      Serial.println(color, HEX);
    }
  }

  server.send ( 200, "text/html", PAGE_Color);
  Serial.println(__FUNCTION__);

}
