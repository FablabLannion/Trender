#ifndef __TDR_PAGE_ADMIN_H__
#define __TDR_PAGE_ADMIN_H__

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

const char PAGE_AdminMainPage[]  = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<strong>Administration</strong>
<hr>
<a href="info"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<!-- <a href="thingspeak"   style="width:250px"  class="btn btn--m btn--blue" >Set ThingSpeak Mode</a><br>-->
<a href="config"   style="width:250px"  class="btn btn--m btn--blue" >TimeKeeper Configuration</a><br> 
<a href="color"   style="width:250px"  class="btn btn--m btn--blue" >Color</a><br>
<a href="start"   style="width:250px"  class="btn btn--m btn--blue" >Start TimeKeeper</a>
<a href="stop"   style="width:250px"  class="btn btn--m btn--blue" >Stop TimeKeeper</a><br>


<script>
window.onload = function ()
{
  load("style.css","css", function()
  {
    load("microajax.js","js", function()
    {
        // Do something after load...
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";

#endif