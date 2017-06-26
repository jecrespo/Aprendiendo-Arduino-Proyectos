// ARDUINO WEB PROGRAMMABLE TIMER v0.1
// AUTHOR: Enrique Crespo <aprendiendoarduino@gmail.com> --> https://github.com/jecrespo
// WEB: https://aprendiendoarduino.wordpress.com/
// GITGUB: https://github.com/jecrespo/Aprendiendo-Arduino-Proyectos
// FUNCTION: A programmable Timer web controlled with up to 4 relay controlled output made with Arduino.
// TECHNOLOGIES: Arduino, Web, Ajax.
// Designed for:
//		* Arduino Ethernet 	http://arduino.cc/en/Main/arduinoBoardEthernet or 
//							http://www.arduino.org/products/boards/4-arduino-boards/arduino-leonardo-eth
//		* Relay Shield v3.0 http://www.seeedstudio.com/depot/Relay-Shield-v30-p-2440.html
//
// Arduino Ethernet
// Microcontroller		ATmega328
// Flash Memory			32 KB (ATmega328) of which 0.5 KB used by bootloader
// SRAM           		2 KB
// EEPROM         		1 KB
// CLOCK SPEED			16 MHz
// Digital i/o Pins		14
// Analog input Pins	6
//
// Arduino Leonardo ETH
// Microcontroller		ATmega32u4
// Flash Memory			32 KB (ATmega32u4) of which 4 KB used by bootloader
// SRAM           		2.5 KB (ATmega32u4)
// EEPROM         		1 KB (ATmega32u4)
// CLOCK SPEED			16 MHz
// Digital i/o Pins		20
// Analog input Pins	6
//
// Used PINS:
// Reserved Pins		Pins 10, 11, 12 y 13 are reserved for interfacing with the Ethernet module.
// On board microSD Pin	4 (SS for the SD) used to store files, is accessible through the SD Library. 
// Relay Pins:	Digital Pins 4 (relay channel 4), 5 (relay channel 3), 6 (relay channel 2) and 7 (relay channel 1)
// NOTE: If use Pin 4 for relay channel 4, can't use integrated microSD card reader on Arduino Ethernet

//----------------------- LIBRARIES ------------------------//
#include <Time.h>		// Arduino Time Library Version 1.4. https://github.com/PaulStoffregen/Time 
						// More info: https://www.pjrc.com/teensy/td_libs_Time.html
#include <Timer.h>		// Arduino Timer library Version 1.3. https://github.com/JChristensen/Timer
#include <SPI.h>    	// For SPI bus in ethernet communication
#include <Ethernet.h>	// Manejo del puerto ethernet 

//----------------------- CONSTANTS ------------------------//
#define NTP_UPDATE 3600000	//NTP update time
#define VERSION_WPT "v1.0"	//Version WEB PROGRAMMABLE TIMER
const String WEEKDAYS_API = "GET /weekdays";
const String HOURS_API = "GET /hours";
const String HOLYDAY_API = "GET /holiday";

____________________>>>>> Seguir aqui
Añade el NTP como una librería si es posible....



//----------------------- VARIABLES ------------------------//
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xCA, 0x59};
byte ip[] = {192, 168, 141, 135};
byte dns_server[] = {8, 8, 8, 8};
byte gateway[] = {192, 168, 141, 129};
byte subnet[] = {255, 255, 255, 128};
byte ntp_server[] = {192, 168, 0, 30};
byte web_server[] = {192, 168, 1, 198};

// NTP
unsigned int localPort = 8888;      // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;	//NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
const long timeZoneOffset = 3600;	//Temporizador para actializar NTP

//Ethernet
EthernetUDP Udp;
EthernetServer server = EthernetServer(80);
EthernetClient peticion_web;  //Cliente para hacer llamadas web

//Estado rele
boolean estado = 0; //estado del rele 0 apagado, 1 encendido
int pin_rele = 4;	//Corresponde a la salida 4 del rele shield

//Temporizador
Timer t;
boolean dia[] = {0, 1, 1, 1, 1, 1, 0};	//Dias de la semana, indica si habilita hora o no. La posicion 0 es el domingo
String hora_on[] = {"08", "08", "08", "08", "08", "08", "08"};
String minuto_on[] = {"00", "00", "00", "00", "00", "00", "00"};
String hora_off[] = {"19", "19", "19", "19", "19", "19", "19"};
String minuto_off[] = {"00", "00", "00", "00", "00", "00", "00"};
String nombresDiaSemana[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
String dia_festivo = "01";
String mes_festivo = "01";
String ano_festivo = "1970"; //Solo almaceno el último festivo, diariamente consulto el siguiente

//--------------------- FUNCTIONS --------------------------//
//Envio de NTP basado en https://www.arduino.cc/en/Tutorial/UdpNtpClient
unsigned long sendNTPpacket(byte ntp_server[4]) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  Udp.beginPacket(ntp_server, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void getTimeAndDate() {
  sendNTPpacket(ntp_server);
  delay(1000);
  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long epoch = highWord << 16 | lowWord;	//segundos desde el 1 de enero de 1900
    epoch = epoch - 2208988800UL + timeZoneOffset;	//Unix time
    setTime(epoch);
    Serial.println(epoch);
  }
  Serial.println("fecha sincronizada");
}

//----------------------- SETUP ----------------------------//
void setup() {
  pinMode(pin_rele, OUTPUT);
  Serial.begin(9600);
  Serial.print(F("Temporizador Madrid "));
  Serial.println(VERSION);
  setTime(0);
  Ethernet.begin(mac, ip, dns_server, gateway, subnet);
  Udp.begin(localPort);
  server.begin();
  t.every(NTP_UPDATE, getTimeAndDate);
  getTimeAndDate();
  //Funciones de horario
  //es posible usar esto y prescindir del timer
  //setSyncProvider(getTimeFunction);  // set the external time provider
  //setSyncInterval(interval);         // set the number of seconds between re-sync
  //Inicalizo los valores
  Serial.println(F("Conectando a BBDD..."));
  if (peticion_web.connect(web_server, 80)) {
    Serial.println("connected");
    peticion_web.println("GET /madridMonitor/inicializa_datos.php HTTP/1.1"); //Lanzo el get y luego en el loop cojo los datos.
    peticion_web.println();
    
  } else {
    Serial.println("connection failed");
  }
  //peticion_web.stop();
}

//----------------------- LOOP -----------------------------//
void loop() {
  t.update();
  Serial.println(F("----------"));
  Serial.print(F("Fecha: "));
  Serial.print(year());
  Serial.print(F("/"));
  Serial.print(month());
  Serial.print(F("/"));
  Serial.print(day());
  Serial.print(F(" "));
  Serial.print(hour());
  Serial.print(F(":"));
  Serial.print(minute());
  Serial.print(F(":"));
  Serial.println(second());
  Serial.print(F("Dia de la semana: "));
  Serial.println(weekday());
  Serial.print(F("Estado Rele"));
  Serial.println(estado);

  Serial.print(F("Estado de la sincronizacion: "));
  Serial.println(timeStatus());  //estado de la sincronización

  time_t fecha_hora = now();  //hora actual
  Serial.print(F("Epoch: "));
  Serial.println(fecha_hora);

  //Consulto festivo
  TimeElements tm_festivo;
  tm_festivo.Second = 0;
  tm_festivo.Minute = 0;
  tm_festivo.Hour = 0;
  tm_festivo.Day = dia_festivo.toInt();
  tm_festivo.Month = mes_festivo.toInt();
  tm_festivo.Year = ano_festivo.toInt() - 1970;
  time_t time_festivo = makeTime(tm_festivo);
  Serial.print(F("time siguiente festivo: "));
  Serial.println(time_festivo);

  /*
  if (time_festivo < fecha_hora + 86400) { //Cuando la fecha es el día siguiente del festivo, consulto en nuevo festivo
    Serial.println(F("Conectando a BBDD..."));
    if (peticion_web.connect(web_server, 80)) {
      Serial.println("connected");
      peticion_web.println("GET /madridMonitor/proximo_festivo.php HTTP/1.1"); //Lanzo el get y luego en el loop cojo los datos.
      peticion_web.println();
    } else {
      Serial.println("connection failed");
    }
    //peticion_web.stop();
  }
  */
  
  if ((dia[weekday() - 1]) && ((fecha_hora < time_festivo) || (fecha_hora > time_festivo + 86400))) { //Si en el dia hay programación o no es festivo, si está a cero no hago nada
    //Calculos los Unix epoch time de on y off
    TimeElements tm_on;
    TimeElements tm_off;
    tm_on.Second = 0;
    tm_on.Minute = minuto_on[weekday() - 1].toInt();
    tm_on.Hour = hora_on[weekday() - 1].toInt();
    tm_on.Day = day();
    tm_on.Month = month();
    tm_on.Year = year() - 1970;

    tm_off.Second = 0;
    tm_off.Minute = minuto_off[weekday() - 1].toInt();
    tm_off.Hour = hora_off[weekday() - 1].toInt();
    tm_off.Day = day();
    tm_off.Month = month();
    tm_off.Year = year() - 1970;

    time_t time_on = makeTime(tm_on);
    time_t time_off = makeTime(tm_off);
    Serial.print(F("time on: "));
    Serial.println(time_on);
    Serial.print(F("time off: "));
    Serial.println(time_off);

    if ((fecha_hora > time_on) && (fecha_hora < time_off)) {  //Si estoy en periodo de encendido
      if (!estado) {
        digitalWrite(pin_rele, HIGH);
        Serial.println(F("ENCIENDO"));
        estado = 1;
      }
    }
    else {
      if (estado) {
        digitalWrite(pin_rele, LOW);
        Serial.println(F("APAGO"));
        estado = 0;
      }
    }
  }
  else {  //Si el día está apagado por seguridad apago en caso de estar encendido. Solo medida seguridad
    if (estado) {
      digitalWrite(pin_rele, LOW);
      Serial.println(F("APAGO"));
      estado = 0;
    }
  }

  //WEB
  EthernetClient client = server.available();
  if (client) {
    String api_request = "";
    while (client.available()) {
      char c = client.read();
      if (c == '\n') break;
      api_request += c;
    }
    while (client.available()) {  //como client.flush() no funciona, vacio el buffer
      char c = client.read();
    }
    Serial.print(F("------------------------>  "));
    Serial.println(api_request);
    if (api_request.startsWith("GET / HTTP")) {
      client.println(F("HTTP/1.0 200K"));
      client.println();
      client.print("<!DOCTYPE html>");
      client.print("<html><head><title>Temporizador Madrid</title>");
      client.print("<style type=\"text/css\">body {color: white;background-color: black;}</style></head>");
      client.print(F("<body>"));
      client.print(F("<h2>Reloj Programador Madrid</h2>"));
      client.print(F("Fecha: "));
      client.print(year());
      client.print(F("/"));
      client.print(month());
      client.print(F("/"));
      client.print(day());
      client.print(F(" "));
      client.print(hour());
      client.print(F(":"));
      client.print(minute());
      client.print(F(":"));
      client.print(second());
      client.print(F("</br>"));
      client.print(F("Estado del rele: "));
      estado ? client.print(F("ENCENDIDO")) : client.print(F("APAGADO"));
      client.print(F("</br>"));
      client.print(F("Proximo festivo: "));
      client.print(ano_festivo);
      client.print(F("/"));
      client.print(mes_festivo);
      client.print(F("/"));
      client.print(dia_festivo);
      if (time_festivo < fecha_hora + 86400) {
        client.print(F(" !!ERROR DE FESTIVO!! "));
      }
      client.print(F("</br>"));
      client.print(F("<h2>Programacion:</h2>"));
      client.print(F("<table border=\"1\" style=\"width:100%\"><tr><th>Dia Semana</th><th>Habilitado</th><th>Hora Encendido</th><th>Hora Apagado</th></tr>"));
      for (int i = 0; i <= 6; i++) {
        client.print(F("<tr><td>"));
        client.print(nombresDiaSemana[i]);
        client.print(F("</td><td>"));
        dia[i] ? client.print(F("ON")) : client.print(F("OFF"));
        client.print(F("</td><td>"));
        if (dia[i]) {
          client.print(hora_on[i]);
          client.print(F(":"));
          client.print(minuto_on[i]);
          client.print(F("</td><td>"));
          client.print(hora_off[i]);
          client.print(F(":"));
          client.print(minuto_off[i]);
          client.print(F("</td></tr>"));
        }
        else {
          client.print(F("</td><td>"));
          client.print(F("</td></tr>"));
        }
      }
      client.print(F("</table></body>"));
      client.print("</html>");
      Serial.println();
    }
    else if (api_request.startsWith(WEEKDAYS_API)) {
      api_request = api_request.substring(WEEKDAYS_API.length(), api_request.indexOf(" HTTP")); //Quito el inicio de API y el HTTP final
      Serial.println(api_request);
      client.println(F("HTTP/1.0 200K"));
      client.println();
      client.println(F("OK"));
      client.stop();
      //Actualizo la variable dia[] = {0, 1, 1, 1, 1, 1, 0};
      Serial.print(F("Actualizados dias de la semana a: "));
      for (int i = 0; i < 7; i++) {
        String s =  (String)api_request.charAt(i * 2 + 1);
        dia[i] = s.toInt();
        Serial.print(dia[i]);
        Serial.print(":");
      }
      Serial.println();
    }
    else if (api_request.startsWith(HOURS_API)) {
      api_request = api_request.substring(HOURS_API.length(), api_request.indexOf(" HTTP/1.1"));
      Serial.println(api_request);
      client.println(F("HTTP/1.0 200K"));
      client.println();
      client.println(F("OK"));
      client.stop();
      int weekday = ((String)api_request.charAt(1)).toInt() - 1;
      String on = api_request.substring(api_request.indexOf("/on/") + 4, api_request.indexOf("/off/"));
      String off = api_request.substring(api_request.indexOf("/off/") + 5);
      Serial.print(F("Se cambia las horas para el "));
      Serial.println(nombresDiaSemana[weekday]);
      Serial.print(F("Hora on: "));
      Serial.println(on);
      Serial.print(F("Hora off: "));
      Serial.println(off);
      hora_on[weekday] = on.substring(0, on.indexOf(":"));
      minuto_on[weekday] = on.substring(on.indexOf(":") + 1);
      hora_off[weekday] = off.substring(0, off.indexOf(":"));
      minuto_off[weekday] = off.substring(off.indexOf(":") + 1);
    }
    else if (api_request.startsWith(FESTIVO_API)) {
      api_request = api_request.substring(FESTIVO_API.length(), api_request.indexOf(" HTTP/1.1"));
      Serial.println(api_request);
      client.println(F("HTTP/1.0 200K"));
      client.println();
      client.println(F("OK"));
      client.stop();
      int indice = api_request.indexOf("/");	//Recorro el string hasta localizar "/"
      dia_festivo = api_request.substring(indice + 1, api_request.indexOf("/", indice + 1));
      indice = api_request.indexOf("/", indice + 1);
      mes_festivo = api_request.substring(indice + 1, api_request.indexOf("/", indice + 1));
      indice = api_request.indexOf("/", indice + 1);
      ano_festivo = api_request.substring(indice + 1);
      Serial.print(F("Proximo festivo cambiado a: "));
      Serial.print(dia_festivo);
      Serial.print(F("/"));
      Serial.print(mes_festivo);
      Serial.print(F("/"));
      Serial.println(ano_festivo);

    }
    else {
      client.println(F("HTTP/1.0 200K"));
      client.println();
      client.println(F("Peticion no registrada"));
      Serial.println(F("Peticion no registrada"));
    }
    client.stop();
  }
}
