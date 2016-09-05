

const char PAGE_Config[] = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Color</strong>
<hr>
<form action="" method="post">
Duration: <input type="text" id="dur" name="dur" length="3"> min<br/>

1st: <input type="text" id="col1" name="col1" length="6"> <input type="text" id="per1" name="per1" length="3"> % <br/>
2nd: <input type="text" id="col2" name="col1" length="6"> <input type="text" id="per2" name="per2" length="3"> % <br/>
3th: <input type="text" id="col3" name="col1" length="6"> <br/>

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

void sendConfigData () {
  uint8_t i;
  
  String values ="";
  values += "dur|" +  (String) (config.dur) + "|div\n";
  values += "col1|" +  (String) (config.colors[0]) + "|div\n";
  values += "col2|" +  (String) (config.colors[1]) + "|div\n";
  values += "col3|" +  (String) (config.colors[2]) + "|div\n";
  values += "per1|" +  (String) (config.per[0]) + "|div\n";
  values += "per2|" +  (String) (config.per[1]) + "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
}

void processConfig ()
{
  if (server.args() > 0) {
    if (server.hasArg("dur")) {
      config.dur = server.arg("dur").toInt();
    }
    if (server.hasArg("col1")) {
      config.colors[0] = server.arg("col1").toInt();
    }
    if (server.hasArg("col2")) {
      config.colors[1] = server.arg("col2").toInt();
    }
    if (server.hasArg("col3")) {
      config.colors[2] = server.arg("col3").toInt();
    }
    if (server.hasArg("per1")) {
      config.per[0] = server.arg("per1").toInt();
    }
    if (server.hasArg("per2")) {
      config.per[1] = server.arg("per2").toInt();
    }
    
  }

  server.send ( 200, "text/html", PAGE_Config);
  Serial.println(__FUNCTION__); 

}

