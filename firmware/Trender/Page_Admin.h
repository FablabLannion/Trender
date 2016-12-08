

//
//  HTML PAGE
//

const char PAGE_AdminMainPage[]  = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<strong>Administration</strong>
<hr>
<a href="config"   style="width:250px"  class="btn btn--m btn--blue" >Configuration</a><br>
<!-- <a href="color"   style="width:250px"  class="btn btn--m btn--blue" >Color</a><br> -->
<a href="info"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="start"   style="width:110px"  class="btn btn--m btn--blue" >Start</a>
<a href="stop"   style="width:110px"  class="btn btn--m btn--blue" >Stop</a><br>


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


