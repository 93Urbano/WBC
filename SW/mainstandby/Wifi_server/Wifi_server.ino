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
uint CANfrequency = 1000000;
bool CANactive = 0;
uint UARTBauds = 115200;
bool UARTactive = 0;
bool CANShow = 0;
bool UARTShow = 0;
bool RTCShow = 0;
bool CANSave = 0;
bool UARTSave = 0;
bool RTCSave = 0;

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
  server.on("/uartcontrol", HTTP_POST, handleUARTControl);
  server.on("/btconfig", HTTP_POST, handleBTControl);
  server.on("/dataconfig", HTTP_POST, handleDATAControl);
  server.on("/datetimeconfig", HTTP_POST, handleDATETIME);
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
  String html = "<html><head><title>ESP32 WBC</title></head><body>";
  html += "<h1>ESP32 WBC</h1>";
  html += "<ul>";
  html += "<li><a href='/Configuration'>Configuration</a></li>";
  html += "<li><a href='/Data Log'>Data Log</a></li>";
  html += "</ul>";
  html += "</body></html>";
  request->send(200, "text/html", html);
}

void handleDataLog(AsyncWebServerRequest *request) {
  String html = "<html><head><title>WBC Datalogger</title></head><body>";
  html += "<h1>ESP32 WBC - Datalogger</h1>";
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
  String html = "<html><head><title>WBC Configuration</title></head><body>";
  html += "<h1>ESP32 WBC - Configuration</h1>";
  html += "<p><strong>CAN configuration:</strong></p>";
  html += "<form id=\"canFrequencyForm\">";
  html += "<label for=\"canFrequencyControl\">CAN Frequency:</label>";
  html += "<select id=\"canFrequencyControl\" name=\"canFrequencyControl\">";
  html += "<option value=\'1M'>1Mbit/s</option>";
  html += "<option value=\'100K'>100Kbit/s</option>";
  html += "</select>";
  html += "<p><label><input id=\"enableCAN\" name=\"canOption\" type=\"radio\" value=\'enable' onclick=\"toggleRadio('enableCAN', 'disableCAN')\" /> Enable CAN </label></p>";
  html += "<p><label><input id=\"disableCAN\" name=\"canOption\" type=\"radio\" value=\'disable' onclick=\"toggleRadio('disableCAN', 'enableCAN')\" /> Disable CAN </label></p>";
  html += "<p><button type='button' onclick='SendCANconfig()'>Send CAN config</button>";
  html += "</form>";

  html += "<form>";
  html += "<p><strong>UART configuration:</strong></p>";
  html += "<form id=\"uartBaudsForm\">";
  html += "<label for=\"uartBaudsControl\">UART Bauds:</label>";
  html += "<select id=\"uartBaudsControl\" name=\"uartBaudsControl\">";
  html += "<option value=\"9600\">9600</option>";
  html += "<option value=\"19200\">19200</option>";
  html += "<option value=\"38400\">38400</option>";
  html += "<option value=\"57600\">57600</option>";
  html += "<option value=\"115200\">115200</option>";
  html += "</select>";
  html += "<p><label><input id=\"enableUART\" name=\"uartOption\" type=\"radio\" value=\'enable' onclick=\"toggleRadio('enableUART', 'disableUART')\" /> Enable UART </label></p>";
  html += "<p><label><input id=\"disableUART\" name=\"uartOption\" type=\"radio\" value=\'disable' onclick=\"toggleRadio('disableUART', 'enableUART')\" /> Disable UART </label></p>";
  html += "<p><button type='button' onclick='SendUARTconfig()'>Send UART config</button>";
  html += "</form>";

  html += "<p><strong>BT configuration:</strong></p>";
  html += "<p><label><input name=\"showCAN\" type=\"radio\" value=\'yes' /> Show CAN </label></p>";
  html += "<p><label><input name=\"showUART\" type=\"radio\" value=\'yes' /> Show UART </label></p>";
  html += "<p><label><input name=\"showRTC\" type=\"radio\" value=\'yes' /> Show RTC </label></p>";
  html += "<p><button type='button' onclick='SendBTconfig()'>Send BT config</button>";

  html += "<p><strong>Datalogger:</strong></p>";
  html += "<p><label><input name=\"saveCAN\" type=\"radio\" value=\"yes\" /> Save CAN </label></p>";
  html += "<p><label><input name=\"saveUART\" type=\"radio\" value=\"yes\" /> Save UART </label></p>";
  html += "<p><label><input name=\"saveRTC\" type=\"radio\" value=\"yes\" /> Save RTC </label></p>";
  html += "<p><button type='button' onclick='SendDATAconfig()'>Send Datalogger config</button>";

  html += "<p><strong>RTC configuration:</strong></p>";
  html += "<label for=\"fechaHora\">Select Date &amp; Time:</label>";
  html += "<input id=\"fechaHora\" name=\"fechaHora\" type=\"datetime-local\" />";
  html += "<p><button type='button' onclick='SendDATETIME()'>Send Date &amp; Time</button>";
  

  html += "<script>";
  html += "function toggleRadio(checkedId, uncheckedId) {";
  html += "  var checkedRadio = document.getElementById(checkedId);";
  html += "  var uncheckedRadio = document.getElementById(uncheckedId);";
  html += "  uncheckedRadio.checked = false;";
  html += "}";
  html += "function SendCANconfig() {";
  html += "  var canFrequencyControl = document.getElementById('canFrequencyControl').value;";
  html += "  var canOption = document.querySelector('input[name=\"canOption\"]:checked');";
  html += "  var canOptionValue = canOption ? canOption.value : '';"; // Obtener el valor del botón de radio seleccionado
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/cancontrol', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('canFrequencyControl=' + canFrequencyControl + '&canOption=' + canOptionValue);"; // Enviar también el valor del botón de radio
  html += "}";
  html += "function SendUARTconfig() {";
  html += "  var uartBaudsControl = document.getElementById('uartBaudsControl').value;";
  html += "  var uartOption = document.querySelector('input[name=\"uartOption\"]:checked');";
  html += "  var uartOptionValue = uartOption ? uartOption.value : '';"; // Obtener el valor del botón de radio seleccionado
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/uartcontrol', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('uartBaudsControl=' + uartBaudsControl + '&uartOption=' + uartOptionValue);"; // Enviar también el valor del botón de radio
  html += "}";
  html += "function SendBTconfig() {";
  html += "  var showCAN = document.querySelector('input[name=\"showCAN\"]:checked');";
  html += "  var showUART = document.querySelector('input[name=\"showUART\"]:checked');";
  html += "  var showRTC = document.querySelector('input[name=\"showRTC\"]:checked');";
  html += "  var showCANValue = showCAN ? showCAN.value : '';";
  html += "  var showUARTValue = showUART ? showUART.value : '';";
  html += "  var showRTCValue = showRTC ? showRTC.value : '';";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/btconfig', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('showCAN=' + showCANValue + '&showUART=' + showUARTValue + '&showRTC=' + showRTCValue);";
  html += "}";
  html += "function SendDATAconfig() {";
  html += "  var saveCAN = document.querySelector('input[name=\"saveCAN\"]:checked');";
  html += "  var saveUART = document.querySelector('input[name=\"saveUART\"]:checked');";
  html += "  var saveRTC = document.querySelector('input[name=\"saveRTC\"]:checked');";
  html += "  var saveCANValue = saveCAN ? saveCAN.value : '';";
  html += "  var saveUARTValue = saveUART ? saveUART.value : '';";
  html += "  var saveRTCValue = saveRTC ? saveRTC.value : '';";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/dataconfig', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('saveCAN=' + saveCANValue + '&saveUART=' + saveUARTValue + '&saveRTC=' + saveRTCValue);";
  html += "}";
  html += "function SendDATETIME() {";
  html += "  var fechaHora = document.getElementById('fechaHora').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/datetimeconfig', true);";
  html += "  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "  xhttp.send('fechaHora=' + fechaHora);";
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
    if (canFrequencyControl == "1M")  CANfrequency = 1000000;
    else if (canFrequencyControl == "100K") CANfrequency = 100000;
    request->send(200, "text/plain", "canFrequencyControl: " + canFrequencyControl);
  }
  if (request->hasParam("canOption", true)) {
    String canOption = request->getParam("canOption", true)->value();
    if (canOption == "enable") {
      CANactive = 1;
    } else if (canOption == "disable") {
      CANactive = 0;
    }
    request->send(200, "text/plain", "canOption: " + canOption);
  }
  else {
    request->send(400, "text/plain", "Invalid Request");
  }
}

void handleUARTControl(AsyncWebServerRequest *request) {
  if (request->hasParam("uartBaudsControl", true)) {
    String uartBaudsControl = request->getParam("uartBaudsControl", true)->value();
    if (uartBaudsControl == "9600") UARTBauds = 9600;
    else if (uartBaudsControl == "19200")  UARTBauds = 19200;
    else if (uartBaudsControl == "38400")  UARTBauds = 38400;
    else if (uartBaudsControl == "57600")  UARTBauds = 57600;
    else if (uartBaudsControl == "115200")  UARTBauds = 115200;
    request->send(200, "text/plain", "uartBaudsControl: " + uartBaudsControl);
  }
  if (request->hasParam("uartOption", true)) {
    String uartOption = request->getParam("uartOption", true)->value();
    if (uartOption == "enable") {
      UARTactive = 1;
    } else if (uartOption == "disable") {
      UARTactive = 0;
    }
    request->send(200, "text/plain", "uartOption: " + uartOption);
  }
  else {
    request->send(400, "text/plain", "Invalid Request");
  }
}

void handleBTControl(AsyncWebServerRequest *request) {
  String showCAN = request->getParam("showCAN", true)->value();
  String showUART = request->getParam("showUART", true)->value();
  String showRTC = request->getParam("showRTC", true)->value();
  if (showCAN == "yes") CANShow = 1;
  else  CANShow = 0;
  if (showUART == "yes") UARTShow = 1;
  else  UARTShow = 0;
  if (showRTC == "yes") RTCShow = 1;
  else  RTCShow = 0;
}

void handleDATAControl(AsyncWebServerRequest *request) {
  String saveCAN = request->getParam("saveCAN", true)->value();
  String saveUART = request->getParam("saveUART", true)->value();
  String saveRTC = request->getParam("saveRTC", true)->value();
  if (saveCAN == "yes") CANSave = 1;
  else  CANSave = 0;
  if (saveUART == "yes") UARTSave = 1;
  else  UARTSave = 0;
  if (saveRTC == "yes") RTCSave = 1;
  else  RTCSave = 0;
}

void handleDATETIME(AsyncWebServerRequest *request) {
  String datetime = request->getParam("fechaHora", true)->value();
  Serial.print("Fecha y hora seleccionada: ");
  Serial.println(datetime);
}