

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
<input type="checkbox" id="hb" name="hb"> Heartbeat <br/>

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
  char str[7];

  String values ="";
  values += "dur|" +  (String) (config.dur) + "|input\n";
  sprintf(str, "%06X", config.colors[0]);
  values += "col1|#" + String(str) + "|input\n";
  sprintf(str, "%06X", config.colors[1]);
  values += "col2|#" + String(str) + "|input\n";
  sprintf(str, "%06X", config.colors[2]);
  values += "col3|#" + String(str) + "|input\n";
  values += "per1|" +  (String) (config.per[0]) + "|input\n";
  values += "per2|" +  (String) (config.per[1]) + "|input\n";
  values += "hb|" +  (String) (config.hb ? "checked" : "") + "|chk\n";
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
      config.colors[0] = strtol(server.arg("col1").substring(1).c_str(), NULL, 16);
    }
    if (server.hasArg("col2")) {
      config.colors[1] = strtol(server.arg("col2").substring(1).c_str(), NULL, 16);
    }
    if (server.hasArg("col3")) {
      config.colors[2] = strtol(server.arg("col3").substring(1).c_str(), NULL, 16);
    }
    if (server.hasArg("per1")) {
      config.per[0] = server.arg("per1").toInt();
    }
    if (server.hasArg("per2")) {
      config.per[1] = server.arg("per2").toInt();
    }
    if (server.hasArg("hb")) {
        config.hb = 1;
    } else {
        config.hb = 0;
    }

  }

  server.send ( 200, "text/html", PAGE_Config);
  Serial.println(__FUNCTION__);
  Serial.println(config.colors[0], HEX);
  Serial.println(config.colors[1], HEX);
  Serial.println(config.colors[2], HEX);
}

