#include <Ethernet.h>
#include <SPI.h>

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
  192, 168, 1, 22
};
char url[] = "www.aprendiendoarduino.com";
byte aprendiendoarduino[] = { 217, 76, 132, 240};

EthernetClient client;
String webString = "";
String nombre = "";
String mensaje = "";

void setup()
{
  Ethernet.begin(mac, ip, DNS, gateway, subnet);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Manda un mensaje al servidor.");
  Serial.println("Introduce Nombre:");
  while (Serial.available() == 0) {
  }
  do {
    char caracter_leido = Serial.read();
    nombre += caracter_leido;
    delay(5);
  }  while (Serial.available() > 0);
  Serial.println(nombre);

  Serial.println("Introduce Mensaje:");
  while (Serial.available() == 0) {
  }
  do {
    char caracter_leido = Serial.read();
    mensaje += caracter_leido;
    delay(5);
  }  while (Serial.available() > 0);
  serial.println(mensaje);

  grabaDatos();
  delay(200);
}

void loop()
{

  if (client.available()) {
    Serial.println("Respuesta del Servidor---->");
    while (client.available()) {
      char c = client.read();
      webString += c
    }
    Serial.println(webString);
    if (webString.endsWith("GRABADOS") == true) Serial.println("Datos guardados correctamente");
    else Serial.println("Error al guardar los datos");

    client.stop();

  }
}

void grabaDatos() {
  Serial.println("enviando mensaje... ");

  Serial.println("connecting to server...");
  if (client.connect(raspberry, 80)) {
    Serial.println("connected");
    client.print("GET /servicio/grabaMensajes.php?nombre=");
    client.print(nombre);
    client.print("&mensaje=");
    client.print(mensaje);
    client.println(" HTTP/1.1");
    client.println("Host: www.aprendiendoarduino.com");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed");
  }
}
