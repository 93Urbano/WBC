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
  server.on("/canControl", HTTP_POST, handleCANControl);
  server.on("/Data Log", HTTP_GET, handleDataLog);
  server.on("/Config", HTTP_GET, handleConfig);

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
  html += "<li><a href='/Config'>Config</a></li>";
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
  String html = "<html><head><title>Nueva Pestaña</title></head><body>";
  html += "<h1>ESP32 UCW - Configuration</h1>";

  // Terminal para escribir strings
  html += "<div id='terminal'>";
  html += "<textarea id='terminalInput' rows='4' cols='50'></textarea><br>";
  html += "<button type='button' onclick='writeOnTerminal()'>Send to terminal</button>";
  html += "<div id='terminalOutput'></div>";
  html += "</div>";

  html += "<form id='canFrequencyForm'>";
  html += "<label for='canFrequencyControl'>CAN Frequency:</label>";
  html += "<select id='canFrequencyControl' name='canFrequencyControl'>";
  html += "<option value='1M'>1Mbit/s</option>";
  html += "<option value='100K'>100Kbit/s</option>";
  html += "</select>";
  html += "<br><br>";
  html += "<button type='button' onclick='sendCANCommand()'>Send CAN config</button>";
  html += "</form>";

  // Script para manejar el terminal
  html += "<script>";
  html += "function writeOnTerminal() {";
  html += "  var input = document.getElementById('terminalInput').value;";
  html += "  var output = document.getElementById('terminalOutput');";
  html += "  output.innerHTML += '<p>' + input + '</p>'";
  html += "  document.getElementById('terminalInput').value = '';"; // Limpiar el input después de enviar
  html += "}";

  // Otras funciones JavaScript
  html += "function otraFuncion() {";
  html += "  // Código de la otra función";
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
      CANfrequency = 1;
    } else if (canFrequencyControl == "100K") {
      CANfrequency = 0;
    }
    request->send(200, "text/plain", "Frecuencia CAN controlada: " + canFrequencyControl);
  } else {
    request->send(400, "text/plain", "Invalid Request");
  }
}
