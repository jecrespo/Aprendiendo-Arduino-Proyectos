//Sala_Reuniones_Sensores
//Fecha Producción: 30/11/2015
//Funcion: http://gitlab/root/Sala_Reuniones_Sensores
//Función: Sistema de monitorizacion de las condiones ambientales de la nueva sala de reuniones de Portalada
//         En función de las condiciones, activar la renovación de aire

//----------------------- LIBRARIES ------------------------//
#include <dht.h>
#include <SPI.h>                  // Manejo del bus SPI que usa el ethernet shield para comunicarse con Arduino.
#include <Ethernet.h>             // Manejo del puerto ethernet 
#include <Timer.h>                // Para lanzar la grabación de datos de forma periodica.

//----------------------- CONSTANTS ------------------------//
#define VERSION "v1.2"
#define DHT22_PIN 7
#define RELE_PIN 8
#define QUALITY_SENSOR_PIN A0
#define DEBUG 1
#define SEND_TIME 60000     //Grabo datos cada 60 segundos

//----------------------- VARIABLES RED ------------------------//
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x70, 0xC1};
byte ip[] = {192, 168, 1, 220};
byte dns_server[] = {192, 168, 0, 5};
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 0};
byte web_server[] = {192, 168, 1, 198};
EthernetServer server = EthernetServer(80);

//----------------------- VARIABLES ------------------------//
int current_quality[10];
int lectura = 0;
dht DHT;
boolean rele = 0; //0 = OFF, 1 = ON
boolean ventilador_manual = 0;  // 1 = que lo conecto manualmente
int umbral_calidad = 80;
Timer t;

//--------------------- FUNCTIONS --------------------------//

void muestroWeb(EthernetClient &client_web) {
  Serial.println(F("Muestro web"));
  client_web.println(F("HTTP/1.1 200 OK"));
  client_web.println(F("Content-Type: text/html"));
  client_web.println();
  client_web.print("<!DOCTYPE html>");  //Si pongo F Chrome me encapsula la web
  client_web.print(F("<html><head><title>Calidad Aire Sala Reuniones Portalada</title>"));
  client_web.print(F("<style type=\"text/css\">body {color: black;background-color: #D7DBDA;}"));
  client_web.print(F("div {background-color: #006666; color: white; border: 1px solid black; padding: 5px; margin: 5px; height: auto;"));
  client_web.print(F("width: 100px; border-radius: 10px; text-align: center; float: left; font-weight:bold;} .cabecera {clear: left;}"));
  client_web.print(F(".formulario {background-color: #004F00; width: auto;}.oculto{background-color: #004F00; border: none;}"));
  client_web.print(F(".OFF {background-color: #006666;} .ON {background-color: red;}"));
  client_web.print(F("input, #boton {width: 80px; border-radius: 5px; font-weight:bold;text-align: center;}#boton:hover{border: 3px solid black;}"));
  client_web.print(F("#boton:hover{border: 3px solid black;} #principal {background-color: #000F00;}"));
  client_web.print(F("</style>"));
  client_web.print(F("<script type=\"text/javascript\">function update(){var xhttp;xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){"));
  client_web.print(F("if (xhttp.readyState == 4 && xhttp.status == 200) {var json = JSON.parse(xhttp.responseText);"));
  client_web.print(F("document.getElementById(\"airQuality\").innerHTML = json.Sala_Reuniones.air_quality;"));
  client_web.print(F("document.getElementById(\"temperatura\").innerHTML = json.Sala_Reuniones.dht22.temperatura;"));
  client_web.print(F("document.getElementById(\"humedad\").innerHTML = json.Sala_Reuniones.dht22.humedad;"));
  client_web.print(F("document.getElementById(\"rele\").innerHTML = json.Sala_Reuniones.rele;"));
  client_web.print(F("document.getElementById(\"rele\").className = json.Sala_Reuniones.rele;"));
  client_web.print(F("document.getElementById(\"boton\").innerHTML = json.Sala_Reuniones.manual;"));
  client_web.print(F("document.getElementById(\"boton\").className = json.Sala_Reuniones.manual;"));
  client_web.print(F("document.getElementById(\"dato_input\").value = json.Sala_Reuniones.umbral;}};"));
  client_web.print(F("xhttp.open(\"GET\", \"http://")); //Imprimo la IP del propio arduino
  for (int i = 0; i < 4; i++) {
    client_web.print(ip[i]);
    if (i < 3) client_web.print(F("."));
  }
  client_web.print(F("/ajax\", true);xhttp.send();}"));
  client_web.print(F("function ventilador_manual(){var xhttp;xhttp = new XMLHttpRequest();var boton = document.getElementById(\"boton\").innerHTML;"));
  client_web.print(F("if (boton == \"ON\" ){boton = \"OFF\";document.getElementById(\"boton\").className = 'OFF';}"));
  client_web.print(F("else {boton = \"ON\";document.getElementById(\"boton\").className = 'ON';}document.getElementById(\"boton\").innerHTML = boton;var url = \"http://"));
  for (int i = 0; i < 4; i++) {
    client_web.print(ip[i]);
    if (i < 3) client_web.print(F("."));
  }
  client_web.print(F("/ventilador/\" + boton; xhttp.open(\"GET\", url, true); xhttp.send();update();}"));
  client_web.print(F("function cambia_umbral(){var xhttp; xhttp = new XMLHttpRequest();var valor= document.getElementById(\"dato_input\").value; var url = \"http://"));
  for (int i = 0; i < 4; i++) {
    client_web.print(ip[i]);
    if (i < 3) client_web.print(F("."));
  }
  client_web.print(F("/umbral/\" + valor; xhttp.open(\"GET\", url, true); xhttp.send();setTimeout(function(){update()},500);}"));
  client_web.print(F("function cambiaIntervalo(tiempo){if (tiempo==0) clearInterval(id_interval);else id_interval = setInterval('update()',tiempo);}"));
  client_web.print(F("var id_interval = setInterval('update()',5000);"));
  client_web.print(F("</script></head><body>"));
  client_web.print(F("<div id = \"principal\" class=\"formulario\"><h2>Calidad Aire Sala Reuniones Portalada</h2><div id=\"datos\" class=\"formulario\">"));
  client_web.print(F("<p>DATOS</p><div class=\"cabecera\">Calidad Aire</div><div id=\"airQuality\">"));
  int calidad = 0;
  for (int i = 0; i < 10; i++) {
    calidad += current_quality[i];
  }
  client_web.print(calidad / 10);
  client_web.print(F("</div><div class=\"cabecera\">Temperatura</div><div id=\"temperatura\">"));
  client_web.print(DHT.temperature);
  client_web.print(F(" &deg;C</div><div class=\"cabecera\">Humedad</div><div id=\"humedad\">"));
  client_web.print(DHT.humidity);
  client_web.print(F(" %</div><div class=\"cabecera\">Ventilador</div><div id=\"rele\" "));
  rele ? client_web.print(F("class=\"ON\">ON")) : client_web.print(F("class=\"OFF\">OFF"));
  client_web.print(F("</div></div><div class=\"formulario\"><p>CONFIGURACION</p><div class=\"cabecera\">Umbral Calidad Aire</div>"));
  client_web.print(F("<div id=\"umbral\" class=\"oculto\"><input id=\"dato_input\" type=\"number\" onchange=\"cambia_umbral()\" value=\""));
  client_web.print(umbral_calidad);
  client_web.print(F("\" onmouseover=\"cambiaIntervalo(0)\" onmouseout=\"cambiaIntervalo(5000)\">"));
  client_web.print(F("</div><div class=\"cabecera\">Ventilador Manual</div><div id=\"boton\" onmouseover=\"cambiaIntervalo(0)\" onmouseout=\"cambiaIntervalo(5000)\""));
  client_web.print(F("onclick=\"ventilador_manual()\" "));
  ventilador_manual ? client_web.print(F("class=\"ON\">ON")) : client_web.print(F("class=\"OFF\">OFF"));
  client_web.println(F("</div></div></div></body></html>"));
  client_web.flush();
  client_web.stop();  //Cierro el cliente
}

void muestroAjax(EthernetClient &client_ajax) {
#if DEBUG
  Serial.println(F("Muestro ajax"));
#endif
  client_ajax.println(F("HTTP/1.1 200 OK"));
  client_ajax.println(F("Content-Type: application/json; charset=utf-8"));
  client_ajax.println();
  client_ajax.print(F("{\"Sala_Reuniones\":{\"dht22\":{\"temperatura\":\""));
  client_ajax.print(DHT.temperature);
  client_ajax.print(F(" &deg;C\",\"humedad\":\""));
  client_ajax.print(DHT.humidity);
  client_ajax.print(F(" %\"},\"air_quality\":\""));
  int calidad = 0;
  for (int i = 0; i < 10; i++) {
    calidad += current_quality[i];
  }
  client_ajax.print(calidad / 10);
  client_ajax.print(F("\",\"rele\":\""));
  rele ? client_ajax.print(F("ON")) : client_ajax.print(F("OFF"));
  client_ajax.print(F("\",\"manual\":\""));
  ventilador_manual ? client_ajax.print(F("ON")) : client_ajax.print(F("OFF"));
  client_ajax.print(F("\",\"umbral\":\""));
  client_ajax.print(umbral_calidad);
  client_ajax.println(F("\"}}"));
  client_ajax.flush();
  client_ajax.stop();  //Cierro el cliente
}

void cambioManual(EthernetClient &client_ajax, String valor) {
  boolean resultado = 1;
#if DEBUG
  Serial.println(valor);
#endif
  if (valor == "ON") {
    ventilador_manual = 1;
    rele = 1;
    digitalWrite(RELE_PIN, rele);
    Serial.println("Enciendo Ventilador Manualmente");
  }
  else if (valor == "OFF") {
    ventilador_manual = 0;
    int calidad = 0;
    for (int i = 0; i < 10; i++) {
      calidad += current_quality[i];
    }
    if (((calidad / 10) < (umbral_calidad - 10)) && rele && !ventilador_manual) //hiteresis = 10 puntos
    {
      rele = 0;   //Solo apago si esto en parámetros
      digitalWrite(RELE_PIN, rele);
      Serial.println("Apago Ventilador");
    }
    Serial.println("Apago Ventilador Manualmente");
  }
  else resultado = 0;
  client_ajax.println(F("HTTP/1.1 200 OK"));
  client_ajax.println(F("Content-Type: text/html\r\n"));
  client_ajax.println();
  resultado ? client_ajax.println(F("OK")) : client_ajax.println(F("KO"));
  client_ajax.flush();
  client_ajax.stop();  //Cierro el cliente
  guardaDatos(); //Guarda datos en BBDD
}

void cambioUmbral(EthernetClient &client_ajax, String valor) {
  boolean resultado = 1;
#if DEBUG
  Serial.println(valor);
#endif
  if ((valor.toInt() > 1000) || (valor.toInt() < 0)) resultado = 0;
  else umbral_calidad = valor.toInt();
  client_ajax.println(F("HTTP/1.1 200 OK"));
  client_ajax.println(F("Content-Type: text/html\r\n"));
  client_ajax.println();
  resultado ? client_ajax.println(F("OK")) : client_ajax.println(F("KO"));
  client_ajax.flush();
  client_ajax.stop();  //Cierro el cliente
  guardaDatos(); //Guarda datos en BBDD
}

void guardaDatos() {
  EthernetClient client;
  int calidad = 0;
  for (int i = 0; i < 10; i++) {
    calidad += current_quality[i];
  }
  calidad = calidad / 10;
  Serial.println(F("Sending data..."));
  if (client.connect(web_server, 80))    {
    Serial.println(F("Connected..."));
    client.println(F("POST /portaladaMonitor/saveDataReuniones.php HTTP/1.1"));
    client.println(F("Host: Arduino"));
    client.println(F("Connection: Close"));
    client.println(F("Content-Type: application/x-www-form-urlencoded"));
    client.println(F("Content-Length: 500"));
    client.println(F(""));
    client.print(F("Temperatura="));
    client.print(DHT.temperature);
    client.print(F("&Humedad="));
    client.print(DHT.humidity);
    client.print(F("&Calidad_Aire="));
    client.print(calidad);
    client.print(F("&Ventilador="));
    rele ? client.print(F("on")) : client.print(F("off"));
    client.print(F("&Ventilador_Manual="));
    ventilador_manual ? client.print(F("on")) : client.print(F("off"));
    client.print(F("&Umbral="));
    client.println(umbral_calidad);
    client.stop();
    client.flush();
    Serial.println(F("Data Saved!"));
  }
  else Serial.println(F("Connection Error!"));

}


//----------------------- SETUP ----------------------------//
void setup()
{
  Serial.begin(9600);
  Serial.println(F("Calidad Aire Nueva Sala Reuniones Portalada."));
  Serial.print(F("Version: "));
  Serial.println(VERSION);
  t.every(SEND_TIME, guardaDatos);
  Ethernet.begin(mac, ip, dns_server, gateway, subnet);
  server.begin();
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, rele);
  pinMode(QUALITY_SENSOR_PIN, INPUT);
  for (int i = 0; i < 10; i++) {
    current_quality[i] = analogRead(QUALITY_SENSOR_PIN);
  }
}

//----------------------- LOOP -----------------------------//
void loop()
{
  t.update();
  //Comprobación de la calidad del aire y temperatura
  DHT.read22(DHT22_PIN);
  current_quality[lectura] = analogRead(QUALITY_SENSOR_PIN);  //el valor de calidad de aire es la media de los 10 últimos valores
  lectura++;

  //Regulación ventilador cada 10 lecturas
  if (lectura > 9) {
    lectura = 0;
    int calidad = 0;
    for (int i = 0; i < 10; i++) {
      calidad += current_quality[i];
    }
#if DEBUG
    Serial.println(calidad);
#endif
    if (((calidad / 10) > umbral_calidad) && !rele && !ventilador_manual) //Solo regulo si el ventilador no está en manual
    {
      rele = 1;
      digitalWrite(RELE_PIN, rele);
      Serial.println("Enciendo Ventilador");
      guardaDatos(); //Guarda datos en BBDD
    }
    if (((calidad / 10) < (umbral_calidad - 10)) && rele && !ventilador_manual) //hiteresis = 10 puntos
    {
      rele = 0;
      digitalWrite(RELE_PIN, rele);
      Serial.println("Apago Ventilador");
      guardaDatos(); //Guarda datos en BBDD
    }
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
      client.read();
    }
#if DEBUG
    Serial.print(F("------------------------>  "));
    Serial.println(request);
#endif
    if (request.startsWith("GET / HTTP")) muestroWeb(client);
    else if (request.startsWith("GET /ajax HTTP")) muestroAjax(client);
    else if (request.startsWith("GET /ventilador/")) cambioManual(client, request.substring(16, request.indexOf(" HTTP/1.1")));
    else if (request.startsWith("GET /umbral/")) cambioUmbral(client, request.substring(12, request.indexOf(" HTTP/1.1")));
    else {
      //NO MUESTRO NADA
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/html\r\n"));
      client.println();
      client.println(F("Peticion no registrada"));
#if DEBUG
      Serial.println(F("Peticion no registrada"));
#endif
      client.flush();
      client.stop();  //Cierro el cliente
    }
  }
}
