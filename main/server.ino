#include "main.hpp"

// Replace with your network credentials
const char* ssid = "Dodgeball";
const char* password = "adamflis";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    button{
        scale: 2;
        margin-top: 40px;
    }
    h4{
        margin-top: 40px;
    }
  </style>
</head>

<body>
  <h2>NCDA Shotclock</h2>
  <button id = "reset" onclick="reset()">RESET</button>
  <br>
  <button id = "revert" onclick="revert()">REVERT</button>
  <h4>
  <label style="margin-bottom: 100px">Clock Length:</label>
  <div style="display: flex; flex-direction: column; align-items: center;">
      <div style="display: flex; align-items: center;">
        <span style="margin-right: 10px;">10</span>
        <label class="switch">
          <input type="checkbox" onchange="toggleLength(this)">
          <span class="slider"></span>
        </label>
        <span style="margin-left: 10px;">15</span>
      </div>
    </div>
    </h4>
    <h4>    
    <label>Count Direction:</label>
    <div style="display: flex; flex-direction: column; align-items: center;">
        <div style="display: flex; align-items: center;">
        <span style="margin-right: 10px;">Up</span>
            <label class="switch">
                <input type="checkbox" onchange="toggleDir(this)">
                <span class="slider"></span>
            </label>
            <span style="margin-left: 10px;">Dn</span>
            </div>
        </div>
    </h4>
    <h4>
      <label>Clock Color:</label>
      <input type="color" onchange="updateColor(this)">
    </h4>
<script>

function reset() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?state=2", true);    
    xhr.send();
}

function revert() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?state=3", true);    
    xhr.send();
}

function toggleLength(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked) { 
    xhr.open("GET", "/update?state=1", true); 
  } else { 
    xhr.open("GET", "/update?state=0", true); 
  }
  xhr.send();
}

function toggleDir(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked) { 
    xhr.open("GET", "/update?state=4", true); 
  }  else { 
    xhr.open("GET", "/update?state=5", true); 
  }
  xhr.send();
}

function updateColor(element) {
  var xhr = new XMLHttpRequest();
  const color = element.value;
  const red = parseInt(color.substr(1, 2), 16);
  const green = parseInt(color.substr(3, 2), 16);
  const blue = parseInt(color.substr(5, 2), 16);
  console.log(`red: ${red}, green: ${green}, blue: ${blue}`);
  xhr.open("GET", `/update?r=${red}&g=${green}&b=${blue}`, true);
  xhr.send();
}

</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  // Serial.println(var);
  // if(var == "SLIDERPLACEHOLDER"){
  //   String buttons ="";
  //   String outputStateValue = outputState();
  //   buttons+= "<h4>Output - GPIO 2 - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
  //   return buttons;
  // }
  return String();
}


void wifi(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);
  server.begin();

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String msg;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam("state")) {
      msg = request->getParam("state")->value();
      update(msg.toInt());      
    } else if (request->hasParam("r")) {
      int red = request->getParam("r")->value().toInt();
      int green = request->getParam("g")->value().toInt();
      int blue = request->getParam("b")->value().toInt();
      updateColor(red, green, blue);
    }
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}
  




