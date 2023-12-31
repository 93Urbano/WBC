#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid     = "Galaxy S20+b24a";
const char* password = "davidjimenez";

AsyncWebServer server(80);

int rpmValue = 0;
int tpsPercentage = 0;
int brakePercentage = 0;
int ClutchState = 0;
int ledPin = 0;
int CANfrequency = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/control", HTTP_POST, handleControl);
  server.on("/cancontrol", HTTP_POST, handleCANControl);
  server.on("/Data Log", HTTP_GET, handleDataLog);
  server.on("/Configuration", HTTP_GET, handleConfig);

  server.begin();
}

void loop() {
  delay(1000);
  rpmValue += 100;
  tpsPercentage += 5;
  brakePercentage += 5;
  ClutchState +=1;
}

void handleRoot(AsyncWebServerRequest *request) {
  String html = "<html><head><title>Pestañas</title></head><body>";
  html += "<h1>ESP32 UCW</h1>";
  html += "<ul>";
  html += "<li><a href='/Configuration'>Configuration</a></li>";
  html += "<li><a href='/Data Log'>Data Log</a></li>";
  html += "</ul>";
  html += "</body></html>";
  request->send(200, "text/html", html);
}

void handleDataLog(AsyncWebServerRequest *request) {
  String html = "<html><head><title>Configuración General</title></head><body>";
  html += "<h1>ESP32 UCW - Datalogger</h1>";
  html += "<form id='controlForm'>";
  html += "<label for='ledControl'>LED Control:</label>";
  html += "<select id='ledControl' name='ledControl'>";
  html += "<option value='on'>LED ON</option>";
  html += "<option value='off'>LED OFF</option>";
  html += "</select>";
  html += "<br><br>";
  html += "<button type='button' onclick='sendLEDCommand()'>Send LED command</button>";
  html += "</form>";
  html += "<p>RPM: <span id='rpmValue'>" + String(rpmValue) + "</span></p>";
  html += "<p>%TPS: <span id='tpsValue'>" + String(tpsPercentage) + "</span></p>";
  html += "<p>%Brake: <span id='brakeValue'>" + String(brakePercentage) + "</span></p>";
  html += "<p>Clutch Status: <span id='ClutchValue'>" + String(ClutchState) + "</span></p>";
  html += "<script>";
  html += "function updateData() {";
  html += "  var rpmSpan = document.getElementById('rpmValue');";
  html += "  var tpsSpan = document.getElementById('tpsValue');";
  html += "  var brakeSpan = document.getElementById('brakeValue');";
  html += "  var clutchSpan = document.getElementById('ClutchValue');";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      var data = this.responseText.split(';');";
  html += "      rpmSpan.innerHTML = data[0];";
  html += "      tpsSpan.innerHTML = data[1];";
  html += "      brakeSpan.innerHTML = data[2];";
  html += "      clutchSpan.innerHTML = data[3];";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/data', true);";
  html += "  xhttp.send();";
  html += "}";
  html += "function sendLEDCommand() {";
  html += "  var ledControl = document.getElementById('ledControl').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/control', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('ledControl=' + ledControl);";
  html += "}";
  html += "setInterval(updateData, 1000);";
  html += "</script>";
  html += "</body></html>";
  request->send(200, "text/html", html);
}

void handleConfig(AsyncWebServerRequest *request) {
  String html = "<html><head><title>UCW Configuration</title></head><body>";
  html += "<h1>ESP32 UCW - Configuration</h1>";
  html += "<p><strong>CAN configuration:</strong></p>";
  html += "<form id=\"canFrequencyForm\">";
  html += "<label for=\"canFrequencyControl\">CAN Frequency:</label>";
  html += "<select id=\"canFrequencyControl\" name=\"canFrequencyControl\">";
  html += "<option value=\'1M'>1Mbit/s</option>";
  html += "<option value=\'100K'>100Kbit/s</option>";
  html += "</select>";
  html += "<p><label><input id=\"enableCAN\" name=\"canOption\" type=\"radio\" value=\'enable' onclick=\"toggleRadio('enableCAN', 'disableCAN')\" /> Enable CAN </label></p>";
  html += "<p><label><input id=\"disableCAN\" name=\"canOption\" type=\"radio\" value=\'disable' onclick=\"toggleRadio('disableCAN', 'enableCAN')\" /> Disable CAN </label></p>";
  //html += "<p><button type=\"button\">Send CAN config</button></p>";
  html += "<p><button type='button' onclick='SendCANconfig()'>Send CAN config</button>";
  html += "</form>";

  html += "<form id='controlForm'>";
  html += "<label for='ledControl'>LED Control:</label>";
  html += "<select id='ledControl' name='ledControl'>";
  html += "<option value='on'>LED ON</option>";
  html += "<option value='off'>LED OFF</option>";
  html += "</select>";
  html += "<br><br>";
  html += "<button type='button' onclick='sendLEDCommand()'>Send LED command</button>";
  html += "</form>";

  html += "<form>";
  html += "<p><strong>UART configuration:</strong></p>";
  html += "<p><label><input id=\"enableUART\" name=\"uartOption\" type=\"radio\" value=\"enable\" onclick=\"toggleRadio('enableUART', 'disableUART')\" /> Enable UART </label></p>";
  html += "<p><label><input id=\"disableUART\" name=\"uartOption\" type=\"radio\" value=\"disable\" onclick=\"toggleRadio('disableUART', 'enableUART')\" /> Disable UART </label></p>";
  html += "</form>";

  html += "<form id=\"uartBaudsForm\">";
  html += "<label for=\"uartBaudsControl\">UART Bauds:</label>";
  html += "<select id=\"uartBaudsControl\" name=\"uartBaudsControl\">";
  html += "<option value=\"9600\">9600</option>";
  html += "<option value=\"19200\">19200</option>";
  html += "<option value=\"38400\">38400</option>";
  html += "<option value=\"57600\">57600</option>";
  html += "<option value=\"115200\">115200</option>";
  html += "</select>";
  html += "<p><button type=\"button\">Send UART config</button></p>";
  html += "</form>";

  html += "<p><strong>BT configuration:</strong></p>";
  html += "<p><label><input name=\"showCAN\" type=\"radio\" value=\"yes\" /> Show CAN </label></p>";
  html += "<p><label><input name=\"showUART\" type=\"radio\" value=\"yes\" /> Show UART </label></p>";
  html += "<p><label><input name=\"showRTC\" type=\"radio\" value=\"yes\" /> Show RTC </label></p>";
  html += "<p><button type=\"button\">Send BT config</button></p>";

  html += "<p><strong>Datalogger:</strong></p>";
  html += "<p><label><input name=\"saveCAN\" type=\"radio\" value=\"yes\" /> Save CAN </label></p>";
  html += "<p><label><input name=\"saveUART\" type=\"radio\" value=\"yes\" /> Save UART </label></p>";
  html += "<p><label><input name=\"saveRTC\" type=\"radio\" value=\"yes\" /> Save RTC </label></p>";
  html += "<p><button type=\"button\">Send Datalogger config</button></p>";

  html += "<p><strong>RTC configuration:</strong></p>";
  html += "<label for=\"fechaHora\">Select Date &amp; Time:</label>";
  html += "<input id=\"fechaHora\" name=\"fechaHora\" type=\"datetime-local\" />";
  html += "<p><button type=\"button\">Send Date &amp; Time</button></p>";

  html += "<script>";
  html += "function toggleRadio(checkedId, uncheckedId) {";
  html += "  var checkedRadio = document.getElementById(checkedId);";
  html += "  var uncheckedRadio = document.getElementById(uncheckedId);";
  html += "  uncheckedRadio.checked = false;";
  html += "}";
  html += "function SendCANconfig() {";
  html += "  var canFrequencyControl = document.getElementById('canFrequencyControl').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/cancontrol', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('canFrequencyControl=' + canFrequencyControl);";
  html += "}";
  html += "function sendLEDCommand() {";
  html += "  var ledControl = document.getElementById('ledControl').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/control', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('ledControl=' + ledControl);";
  html += "}";
  html += "</script>";
  html += "</body></html>";
  request->send(200, "text/html", html);
}



void handleData(AsyncWebServerRequest *request) {
  String data = String(rpmValue) + ";" + String(tpsPercentage) + ";" + String(brakePercentage) + ";" + String(ClutchState);
  request->send(200, "text/plain", data);
}

void handleControl(AsyncWebServerRequest *request) {
  if (request->hasParam("ledControl", true)) {
    String ledControl = request->getParam("ledControl", true)->value();
    if (ledControl == "on") {
      digitalWrite(ledPin, HIGH);
    } else if (ledControl == "off") {
      digitalWrite(ledPin, LOW);
    }
    request->send(200, "text/plain", "LED controlado: " + ledControl);
  } else {
    request->send(400, "text/plain", "Invalid Request");
  }
}

void handleCANControl(AsyncWebServerRequest *request) {
  if (request->hasParam("canFrequencyControl", true)) {
    String canFrequencyControl = request->getParam("canFrequencyControl", true)->value();
    if (canFrequencyControl == "1M") {
      digitalWrite(ledPin, HIGH);
      CANfrequency = 1000000;
    } else if (canFrequencyControl == "100K") {
      digitalWrite(ledPin, LOW);
      CANfrequency = 100000;
    }
    request->send(200, "text/plain", "LED controlado: " + canFrequencyControl);
  } else {
    request->send(400, "text/plain", "Invalid Request");
  }
}
