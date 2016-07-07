#include <Ethernet.h>
#include <SPI.h>
#include "Timer.h"


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
byte ip[] = {
  192, 168, 1, 179
};
byte DNS[] = {
  8, 8, 8, 8
};
byte gateway[] = {
  192, 168, 1, 1
};
byte subnet[] = {
  255, 255, 255, 0
};
byte raspberry[] = {
  192, 168, 1, 169
};
char url[] = "www.aprendiendoarduino.com";

EthernetClient client;
Timer t;
Timer t2;
String webString = "";

void setup()
{
  Ethernet.begin(mac, ip, DNS, gateway, subnet);
  Serial.begin(9600);
  delay(1000);
  t.every(5000, grabaDatos);
  t2.every(30000, grabaDatosPublico);
}

void loop()
{
  t.update();
  t2.update();
}

void grabaDatos() {
  Serial.println("leyendo temperatura... ");
  int sensorVal = analogRead(A0);
  float voltage = (sensorVal / 1024.0) * 5.0;
  float temperature = (voltage - 0.5) * 100;
  Serial.print("Leido: ");
  Serial.print(temperature);
  Serial.println(" grados");

  Serial.println("connecting to Raspberry Pi...");
  if (client.connect(raspberry, 80)) {
    Serial.println("connected");
    client.print("GET /grabaDatos.php?arduino=99&temperatura=");
    client.print(temperature);
    client.println(" HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
    delay (1000);
  }
  else {
    Serial.println("connection failed");
  }
  webString = "";
  if (client.available()) {
    Serial.println("Respuesta del Servidor---->");
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
    Serial.println(webString);
    if (webString.indexOf("GRABADOS") >= 0) Serial.println("Datos guardados correctamente");
    else Serial.println("Error al guardar los datos");
    client.stop();
  }
}

void grabaDatosPublico() {
  Serial.println("leyendo temperatura... ");
  int sensorVal = analogRead(A0);
  float voltage = (sensorVal / 1024.0) * 5.0;
  float temperature = (voltage - 0.5) * 100;
  Serial.print("Leido: ");
  Serial.print(temperature);
  Serial.println(" grados");

  Serial.println("connecting to public server...");
  if (client.connect(url, 80)) {
    Serial.println("connected");
    client.print("GET /servicios/grabaDatos.php?arduino=99&dato=");
    client.print(temperature);
    client.println(" HTTP/1.1");
    client.println("Host: www.aprendiendoarduino.com");
    client.println("Connection: close");
    client.println();
    delay (1000);
  }
  else {
    Serial.println("connection failed");
  }
  webString = "";
  if (client.available()) {
    Serial.println("Respuesta del Servidor---->");
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
    Serial.println(webString);
    if (webString.indexOf("GRABADOS") >= 0) Serial.println("Datos guardados correctamente");
    else Serial.println("Error al guardar los datos");
    client.stop();
  }
}
