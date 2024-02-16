#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

//CAN
#include "driver/gpio.h"
#include "driver/twai.h"

// Definir el mensaje CAN
twai_message_t can_message;

const char* ssid     = "Galaxy S20+b24a";
const char* password = "davidjimenez";

//const char *ssid = "vodafone1B2C";
//const char *password = "jimenezurbano";

AsyncWebServer server(80);

int rpmValue = 0;
int tpsPercentage = 0;
int brakePercentage = 0;
int ClutchState = 0;
int ledPin = 0;
int CANfrequency = 0;
int floors = 0; // Variable global para almacenar el número de pisos
bool HallCallsEnable = 0;
bool CarCallsEnable = 0;


void CANInit() {
  //CAN
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_46, GPIO_NUM_3, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  //Install TWAI driverf
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    printf("Driver installed\n");
  } else {
    printf("Failed to install driver\n");
    return;
  }
  //Start TWAI driver
  if (twai_start() == ESP_OK) {
    printf("Driver started\n");
  } else {
    printf("Failed to start driver\n");
    return;
  }
}

void CANSendMsg(uint id, char data_length, char d0, char d1, char d2, char d3, char d4, char d5, char d6, char d7) {
  // Configurar el mensaje CAN
  can_message.identifier = id;                 // ID del mensaje CAN
  can_message.flags = TWAI_MSG_FLAG_EXTD;      // Bandera de mensaje extendido
  can_message.data_length_code = data_length;  // Longitud de los datos
  can_message.data[0] = d0;
  can_message.data[1] = d1;
  can_message.data[2] = d2;
  can_message.data[3] = d3;
  can_message.data[4] = d4;
  can_message.data[5] = d5;
  can_message.data[6] = d6;
  can_message.data[7] = d7;
  /*for (int i = 0; i < can_message.data_length_code; i++) {
    can_message.data[i] = i; // Datos del mensaje
  }*/
  // Enviar el mensaje CAN
  if (twai_transmit(&can_message, portMAX_DELAY) == ESP_OK) {
    Serial.println("Mensaje CAN enviado correctamente");
  } else {
    Serial.println("Error al enviar el mensaje CAN");
  }
}

void HallCallCycle(uint floors) {
  uint call_id = 0x280;
  uint front_id = 0x000;
  uint rear_id = 0x400;
  uint id;
  uint data_length = 6;
  char LSP = 0b00010100;
  char version = 0x00;
  for (uint i = 0; i <= floors; i++) {
    id = call_id + i + front_id;
    CANSendMsg(id, data_length, LSP, i, 0x00, 0x03, 0x01, version, 0, 0);  //press down front
    delay(100);
    CANSendMsg(id, data_length, LSP, i, 0x00, 0x03, 0x01, version, 0, 0);  //release down front
    delay(1000);
  }
}

void CarCallCycle(uint floors) {
  uint call_id = 0x300;
  uint front_id = 0x000;
  uint rear_id = 0x400;
  uint id = call_id + floors + front_id;
  uint data_length = 6;
  char LSP = 0b00010100;
  char version = 0x00;
  for (uint i = 0; i <= floors; i++) {
    CANSendMsg(id, data_length, LSP, i, 0x00, 0x03, 0x01, version, 0, 0);  //press down front
    delay(100);
    CANSendMsg(id, data_length, LSP, i, 0x00, 0x03, 0x01, version, 0, 0);  //release down front
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  CANInit();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //TEST
  server.on("/", HTTP_GET, handleRoot);

  server.on("/cycleHallCalls", HTTP_GET, [](AsyncWebServerRequest *request){
    // Cambia el valor del booleano global
    HallCallsEnable = !HallCallsEnable;
    // Envía una respuesta al cliente con el nuevo valor del booleano
    request->send(200, "text/plain", "Estado del HallCallsEnable: " + String(HallCallsEnable ? "true" : "false"));
  });

  /*server.on("/cycleHallCalls", HTTP_GET, [](AsyncWebServerRequest *request){
    // Lógica para ejecutar HallCallCycle();
    request->send(200, "text/plain", "HallCallCycle(floors) ejecutado");
  });*/

  server.on("/cycleCarCalls", HTTP_GET, [](AsyncWebServerRequest *request){
    // Lógica para ejecutar HallCarCycle();
    request->send(200, "text/plain", "HallCarCycle(floors) ejecutado");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<button onclick=\"toggleButton()\">Toggle Button</button>"); // Crear un botón HTML para controlar el botón físico
  });

   server.on("/setFloors", HTTP_POST, [](AsyncWebServerRequest *request){
    // Obtener el valor de floors desde la solicitud POST
    if (request->hasParam("floors", true)) {
      AsyncWebParameter* p = request->getParam("floors", true);
      extern int floors; // Capturar la variable floors
      floors = p->value().toInt();
      Serial.print("Número de pisos guardado: ");
      Serial.println(floors);
      request->send(200, "text/plain", "Número de pisos guardado correctamente");
    } else {
      request->send(400, "text/plain", "Parámetro 'floors' no encontrado");
    }
  });

  // Iniciar el servidor
  server.begin();
}

void loop() {
  digitalWrite(ledPin, HIGH);
  if (HallCallsEnable){
  digitalWrite(ledPin, HIGH);
    HallCallCycle(floors);
  }
  if (CarCallsEnable)
    CarCallCycle(floors);
  //digitalWrite(ledPin, LOW);
  delay(1000);
  HallCallCycle(floors);
}



void handleRoot(AsyncWebServerRequest *request) {
  String html ="<!DOCTYPE html>\n";
  html += "<html lang=\"en\">\n";
  html += "<head>\n";
  html += "    <meta charset=\"UTF-8\">\n";
  html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  html += "    <title>Call Cycle Board</title>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "    <h1>Call Cycle Board</h1>\n";
  html += "    <form id=\"inputForm\">\n";
  html += "        <label for=\"floorsInput\">Number of floors:</label>\n";
  html += "        <input type=\"number\" id=\"floorsInput\" name=\"floorsInput\">\n";
  html += "        <button type=\"submit\" id=\"saveButton\">Save</button>\n";
  html += "    </form>\n";
  html += "    <button onclick=\"cycleHallCalls()\">Cycle Hall Calls</button>\n";
  html += "    <button onclick=\"cycleCarCalls()\">Cycle Car Calls</button>\n";
  html += "\n";
  html += "    <script>\n";
  html += "        function cycleHallCalls() {\n";
  html += "            // Cambia el valor del booleano global\n";
  html += "            HallCallsEnable = !HallCallsEnable;\n";
  html += "            console.log('Estado del booleano:', HallCallsEnable);\n";
  html += "        }\n";
  html += "\n";
  html += "        function cycleCarCalls() {\n";
  html += "            // Lógica para ejecutar HallCarCycle();\n";
  html += "            // Puedes hacer una petición AJAX al servidor si es necesario\n";
  html += "            // Ejemplo:\n";
  html += "            fetch('/cycleCarCalls')\n";
  html += "                .then(response => {\n";
  html += "                    if (response.ok) {\n";
  html += "                        console.log('HallCarCycle() ejecutado exitosamente');\n";
  html += "                    } else {\n";
  html += "                        console.error('Error al ejecutar HallCarCycle()');\n";
  html += "                    }\n";
  html += "                })\n";
  html += "                .catch(error => console.error('Error de red:', error));\n";
  html += "        }\n";
  html += "\n";
  html += "        document.getElementById(\"inputForm\").addEventListener(\"submit\", function(event) {\n";
  html += "            event.preventDefault();\n";
  html += "            var floors = document.getElementById(\"floorsInput\").value;\n";
  html += "            fetch('/setFloors', {\n";
  html += "                method: 'POST',\n";
  html += "                headers: {\n";
  html += "                    'Content-Type': 'application/x-www-form-urlencoded',\n";
  html += "                },\n";
  html += "                body: 'floors=' + floors,\n";
  html += "            })\n";
  html += "            .then(response => response.text())\n";
  html += "            .then(data => {\n";
  html += "                console.log(data);\n";
  html += "            })\n";
  html += "            .catch(error => console.error('Error:', error));\n";
  html += "        });\n";
  html += "    </script>\n";
  html += "</body>\n";
  html += "</html>\n";
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
  html += "  document.getElementById('terminalInput').value = '';";  // Limpiar el input después de enviar
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


