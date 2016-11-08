//Sala_Reuniones_Sensores
//Funcion: http://gitlab/root/Sala_Reuniones_Sensores
//Función:  Sistema de monitorizacion de las condiones ambientales de la nueva sala de reuniones de Portalada
//          En función de las condiciones, activar la renovación de aire

//----------------------- LIBRARIES ------------------------//
#include <AirQuality.h>
#include <dht.h>
#include <SPI.h>                  // Manejo del bus SPI que usa el ethernet shield para comunicarse con Arduino.
#include <Ethernet.h>             // Manejo del puerto ethernet 

//----------------------- CONSTANTS ------------------------//
#define VERSION "v1.0_dev_eth"
#define DHT22_PIN 7
#define RELE_PIN 8
#define QUALITY_SENSOR_PIN A0

//----------------------- VARIABLES RED ------------------------//
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xCA, 0x52};
byte ip[] = {192, 168, 1, 179};
byte dns_server[] = {8, 8, 8, 8};
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 1};
byte web_server[] = {192, 168, 1, 198};
EthernetServer server = EthernetServer(80);

//----------------------- VARIABLES ------------------------//
AirQuality airqualitysensor;
int current_quality = -1;
dht DHT;

//--------------------- FUNCTIONS --------------------------//
void muestra_web(EthernetClient &client_connected) {
  char buffer_char[30];	//Buffer temporal
  char tBuffer[500];	//Buffer de envío de líneas
  Serial.println("Muestro web");
  //
  strcpy_P(tBuffer, PSTR("HTTP/1.1 200 OK\r\n"));
  strcat_P(tBuffer, PSTR("Content-Type: text/html\r\n"));
  strcat_P(tBuffer, PSTR("Connection: close\r\n"));
  strcat_P(tBuffer, PSTR("server: arduino_sala_reuniones\r\n"));
  strcat_P(tBuffer, PSTR("\r\n"));
  strcat_P(tBuffer, PSTR("<!DOCTYPE html>"));
  strcat_P(tBuffer, PSTR("<html><head><title>Calidad Aire Sala Reuniones Portalada</title>\r\n"));
  client_connected.write(tBuffer);
  //
  strcpy_P(tBuffer, PSTR("<style type=\"text/css\">body {color: black;background-color: #D7DBDA;}"));
  strcat_P(tBuffer, PSTR("div {background-color: #006666; color: white; border: 1px solid black; padding: 5px; margin: 5px; height: auto;"));
  strcat_P(tBuffer, PSTR("width: 100px; border-radius: 10px; text-align: center; float: left; font-weight:bold;} .cabecera {clear: left;}"));
  strcat_P(tBuffer, PSTR("</style>\r\n"));
  client_connected.write(tBuffer);
  //
  strcpy_P(tBuffer, PSTR("<script type=\"text/javascript\">function update(){var xhttp;xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){"));
  strcat_P(tBuffer, PSTR("if (xhttp.readyState == 4 && xhttp.status == 200) {var json = JSON.parse(xhttp.responseText);"));
  strcat_P(tBuffer, PSTR("document.getElementById(\"airQuality\").innerHTML = json.Sala_Reuniones.air_quality;\r\n"));
  client_connected.write(tBuffer);
  //
  strcpy_P(tBuffer, PSTR("document.getElementById(\"temperatura\").innerHTML = json.Sala_Reuniones.dht22.temperatura;"));
  strcat_P(tBuffer, PSTR("document.getElementById(\"humedad\").innerHTML = json.Sala_Reuniones.dht22.humedad;}};"));
  strcat_P(tBuffer, PSTR("xhttp.open(\"GET\", \"http://192.168.1.179/ajax\", true);xhttp.send();}setInterval('update()',5000);")); //*****pon la variable IP******
  strcat_P(tBuffer, PSTR("</script>\r\n"));
  client_connected.write(tBuffer);
  //
  strcpy_P(tBuffer, PSTR("</head><body>\r\n"));
  strcat_P(tBuffer, PSTR("<h2>Calidad Aire Sala Reuniones Portalada</h2><p>DEMO en mesa Enrique</p>\r\n"));
  strcat_P(tBuffer, PSTR("<div class=\"cabecera\">Calidad Aire</div><div id=\"airQuality\">"));
  Serial.println(airqualitysensor.first_vol);
  Serial.println(String(airqualitysensor.first_vol));
  String(airqualitysensor.first_vol).toCharArray(buffer_char, 30);
  Serial.println(buffer_char);
  strcat(tBuffer, buffer_char);
  strcat_P(tBuffer, PSTR("</div><div class=\"cabecera\">Temperatura</div><div id=\"temperatura\">"));
  String(DHT.temperature).toCharArray(buffer_char, 30);
  strcat(tBuffer, buffer_char);
  strcat_P(tBuffer, PSTR(" &deg;C</div><div class=\"cabecera\">Humedad</div><div id=\"humedad\">"));
  String(DHT.humidity).toCharArray(buffer_char, 30);
  strcat(tBuffer, buffer_char);
  strcat_P(tBuffer, PSTR(" %</div>\r\n"));
  strcat_P(tBuffer, PSTR("</body></html>\r\n"));
  client_connected.write(tBuffer);
  delay(50);
  client_connected.stop();
}

void muestra_ajax(EthernetClient &client_connected) {
  char buffer_char[30];	//Buffer temporal
  char tBuffer[400];	//Buffer de envío de líneas
  Serial.println(F("Muestro ajax"));
  strcpy_P(tBuffer, PSTR("HTTP/1.1 200 OK\r\n"));
  strcat_P(tBuffer, PSTR("Content-Type: application/json; charset=utf-8\r\n"));
  strcat_P(tBuffer, PSTR("Connection: close\r\n"));
  strcat_P(tBuffer, PSTR("\r\n"));
  strcat_P(tBuffer, PSTR("{\"Sala_Reuniones\":{\"dht22\":{\"temperatura\":\""));
  String(DHT.temperature).toCharArray(buffer_char, 30);
  strcat(tBuffer, buffer_char);
  strcat_P(tBuffer, PSTR(" &deg;C\",\"humedad\":\""));
  String(DHT.humidity).toCharArray(buffer_char, 30);
  strcat(tBuffer, buffer_char);
  strcat_P(tBuffer, PSTR(" %\"},\"air_quality\":\""));
  String(airqualitysensor.first_vol).toCharArray(buffer_char, 30);
  strcat(tBuffer, buffer_char);
  strcat_P(tBuffer, PSTR("\"}}\r\n"));
  client_connected.write(tBuffer);
  delay(50);
  client_connected.stop();
}

//----------------------- SETUP ----------------------------//
void setup()
{
  Serial.begin(9600);
  Serial.println("Calidad Aire Nueva Sala Reuniones Portalada. ");
  Serial.print("Version: ");
  Serial.println(VERSION);
  airqualitysensor.init(QUALITY_SENSOR_PIN);
  Ethernet.begin(mac, ip, dns_server, gateway, subnet);
  server.begin();
}

//----------------------- LOOP -----------------------------//
void loop()
{
  //Comprobación de la calidad del aire y temperatura
  current_quality = airqualitysensor.slope();
  DHT.read22(DHT22_PIN);

  if (current_quality >= 0)// if a valid data returned.
  {
    if (current_quality == 0)
      Serial.println("High pollution! Force signal active");
    else if (current_quality == 1)
      Serial.println("High pollution!");
    else if (current_quality == 2)
      Serial.println("Low pollution!");
    else if (current_quality == 3)
      Serial.println("Fresh air");
    Serial.print("Temperatura: ");
    Serial.println(DHT.temperature);
    Serial.print("Humedad: ");
    Serial.println(DHT.humidity);
  }

  //WEB
  EthernetClient client = server.available(); //Cliente conectado al servidor
  if (client) {
    String request = "";
    while (client.available()) {
      char c = client.read();
      if (c == '\n') break;
      else request += c;
    }
    while (client.available()) {  //como client.flush() no funciona, vacio el buffer
      char c = client.read();
    }
    Serial.print("------------------------>  ");
    Serial.println(request);
    if (request.startsWith("GET / HTTP")) muestra_web(client);
    else if (request.startsWith("GET /ajax HTTP")) muestra_ajax(client);
    else {
      char tBuffer[400];
      strcpy_P(tBuffer, PSTR("HTTP/1.1 200 OK\r\n"));
      strcat_P(tBuffer, PSTR("Content-Type: text/html\r\n"));
      strcat_P(tBuffer, PSTR("Connection: close\r\n"));
      strcat_P(tBuffer, PSTR("\r\n"));
      strcat_P(tBuffer, PSTR("Peticion no registrada\r\n"));
      client.write(tBuffer);
      delay(50);
      client.stop();
    }
  }
}

//Interrupcion con timer2
ISR(TIMER2_OVF_vect)
{
  if (airqualitysensor.counter == 122) //set 2 seconds as a detected duty
  {

    airqualitysensor.last_vol = airqualitysensor.first_vol;
    airqualitysensor.first_vol = analogRead(A0);
    airqualitysensor.counter = 0;
    airqualitysensor.timer_index = 1;
    PORTB = PORTB ^ 0x20;
  }
  else
  {
    airqualitysensor.counter++;
  }
}
