#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 
  192, 168, 1, 179 };
byte DNS[] = {
  8,8,8,8};
byte gateway[] = {
  192, 168, 1, 1};
byte subnet[] = {
  255,255,255,0};
byte raspberry[] = {
  192,168,1,169}; 

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
  while (Serial.available() == 0){
  }
  do{
    char caracter_leido = Serial.read();
    nombre += caracter_leido;
    delay(5);
  }  while (Serial.available() > 0);
  
  Serial.println("Introduce Mensaje:");
  while (Serial.available() == 0){
  }
  do{
    char caracter_leido = Serial.read();
    mensaje += caracter_leido;
    delay(5);
  }  while (Serial.available() > 0);
  grabaDatos();
  
  while (client.available() == 0){
  }
  Serial.println("Respuesta del Servidor---->");
  do{
	char c = client.read();
    webString += c;
  }  while (client.available() > 0);
  Serial.println(webString);
  if (webString.endsWith("GRABADOS") == true) Serial.println("Datos guardados correctamente");
  else Serial.println("Error al guardar los datos");
   
  client.stop();
}

void loop()
{

}

void grabaDatos(){
  Serial.println("enviando mensaje... ");
  
  Serial.println("connecting to server...");
  if (client.connect(raspberry, 80)) {
    Serial.println("connected");
    client.print("GET /grabaMensajes.php?nombre=");
    client.print(nombre);
	client.print("&mensaje=");
	client.print(mensaje);
    client.println(" HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed");
  }
}
