#ifndef __TDR_PAGE_CONFIG_H__
#define __TDR_PAGE_CONFIG_H__

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


const char PAGE_Config[] = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script>
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Configuration</strong>
<hr>
<form action="" method="post">
Duration: <input type="number" id="dur" name="dur" maxlength="3" size="3" min="1"> minutes<br/>

1st: <input type="color" id="col1" name="col1"> <input type="number" id="per1" name="per1" maxlength="3" size="3" max="99"> % <br/>
2nd: <input type="color" id="col2" name="col2"> <input type="number" id="per2" name="per2" maxlength="3" size="3" max="100"> % <br/>
3th: <input type="color" id="col3" name="col3"> <br/>
<!--<input type="checkbox" id="hb" name="hb"> Heartbeat <br/>-->

<input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Set">
</form>
<script>
window.onload = function ()
{
  load("style.css","css", function()
  {
    load("microajax.js","js", function()
    {
        setValues("/config/values");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";

#endif

