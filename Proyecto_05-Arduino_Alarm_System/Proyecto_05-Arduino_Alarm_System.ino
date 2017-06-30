#include "Timer.h"                     //http://github.com/JChristensen/Timer
#include <Ethernet.h>
#include <SPI.h>

#define DEBUG 1
#define NOTE_C5  523  //Frecuencia de sonido del buzzer

#define TIEMPO_PRE_ALARMA 10000 //Tiempo que permanece en pre-alarma
#define TIEMPO_LDR 5000         //Tiempo en que el LDR está activo antes de pasar a pre-alarma
#define UMBRAL_LDR 900          //Umbral de disparo de prealarma del LDR (AJUSTAR el valor del umbral para que detecte la alarma según iluminación recinto)

//ESTADOS
#define NORMAL 0
#define PRE_ALARMA 1
#define ALARMA 2

//PINES
#define PIN_TILT 2
#define PIN_LED 3
#define PIN_BOTON 4
#define PIN_BUZZER 5
#define PIN_LDR A0

//PINES TECLADO
// Simulo teclado usando el monitor serie

//PASSWORD
#define PASSWORD "761254"
#define PIN "0000"  //Pin de accesos a la API
#define TELEFONO "600000000"

//Timer
Timer t;
int timer_id;

//Variable con el estado
int estado = NORMAL; //Inicio en estado normal

//Variables temporales
unsigned long tiempo_deteccion = 0;
unsigned long tiempo_prealarma = 0;

//Network
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xYY};  //Sustituir YY por el numero de MAC correcto
char server[] = "www.aprendiendoarduino.com";
EthernetClient client;

void setup() {
  Serial.begin(9600);
#if DEBUG
  Serial.println("Inicializando...");
  Serial.println("inicializando red...");
#endif

  //Network initialitation
  //Ethernet.begin(mac, ip, DNS, gateway, subnet);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.print("IP asignada por DHCP: ");
    Serial.println(Ethernet.localIP());
  }

  //pin initialitation
  pinMode(PIN_TILT, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BOTON, INPUT_PULLUP);

  //inicializo el estado
  comprueba_tilt(NORMAL);
  comprueba_ldr(NORMAL);

#if DEBUG
  Serial.println("Sistema en Servicio.");
  Serial.println("-----------------");
#endif
}

void loop() {
  t.update();
  switch (estado) {
    case NORMAL:
      comprueba_tilt(NORMAL);
      comprueba_ldr(NORMAL);
      break;
    case PRE_ALARMA:
      lee_teclado();
      break;
    case ALARMA:
      if (!comprueba_tilt(ALARMA) && !comprueba_ldr(ALARMA) && comprueba_boton())
        actualiza_alarma(NORMAL);
      break;
  }
#if DEBUG
  imprime_valiables();
#endif
}

boolean comprueba_tilt(int valor_estado) {
  //Devuelve 1 si está activo y 0 si no está activo
  int valor_tilt = digitalRead(PIN_TILT); //es un INPUT_PULLUP activo = 0
#if DEBUG
  Serial.println("Valor_tilt = " + (String)valor_tilt);
#endif

  //NORMAL
  if ((valor_estado == NORMAL) && (valor_tilt)) { //Tilt activado en vertical leo 1 al poner en vertical
    actualiza_alarma(PRE_ALARMA);
    return 1;
  }

  //ALARMA
  if ((valor_estado == ALARMA) && (valor_tilt))  //Tilt activado en vertical leo 1
    return 1;
  if ((valor_estado == ALARMA) && (!valor_tilt)) //Tilt normal, en horizontal lee GND
    return 0;
}

boolean comprueba_ldr(int valor_estado) {
  //Devuelve 1 si está activo y 0 si no está activo
  //ldr a mayor luz menos resistencia
  int valor_ldr = analogRead(PIN_LDR);
#if DEBUG
  Serial.println("Valor_ldr = " + (String)valor_ldr);
#endif

  //NORMAL
  if ((valor_estado == NORMAL) && (valor_ldr > UMBRAL_LDR)) {
    if (tiempo_deteccion == 0)
      tiempo_deteccion = millis();
    else if ((millis() - tiempo_deteccion) > TIEMPO_LDR) {
      tiempo_deteccion = 0;
      actualiza_alarma(PRE_ALARMA);
    }
#if DEBUG
    if (tiempo_deteccion > 0)
      Serial.println("tiempo_deteccion LDR ALARMA = " + (String)(millis() - tiempo_deteccion));
#endif
    return 1;
  }

  if ((valor_estado == NORMAL) && (valor_ldr <= UMBRAL_LDR)) {
    tiempo_deteccion = 0;
    return 0;
  }

  //ALARMA
  if ((valor_estado == ALARMA) && (valor_ldr > UMBRAL_LDR))
    return 1;

  if ((valor_estado == ALARMA) && (valor_ldr <= UMBRAL_LDR))
    return 0;
}

void lee_teclado() {
  //compruebo si ha pasado el tiempo para alarma
  if ((millis() - tiempo_prealarma) > TIEMPO_PRE_ALARMA) {
    tiempo_prealarma = 0;
    actualiza_alarma(ALARMA);
    return 0;
  }

#if DEBUG
  Serial.println("tiempo_prealarma = " + (String)(millis() - tiempo_prealarma));
  Serial.println("Leo Teclado");
#endif
  Serial.println("Introduce clave para desactivar la prealarma");

  if (Serial.available() > 0) {
    String clave = "";
    do {
      clave = clave + (char)Serial.read();
      delay(5);
    }
    while (Serial.available() > 0);
#if DEBUG
    Serial.println("Clave introducida: " + clave);
#endif
    if (clave.startsWith(PASSWORD)) {
      Serial.println("Clave Correcta");
      actualiza_alarma(NORMAL);
    }
    else
      Serial.println("Clave Incorrecta");
  }
}

boolean comprueba_boton() {
  //Devuelve 1 si está activo y 0 si no está activo
  int valor_boton = digitalRead(PIN_BOTON);
#if DEBUG
  Serial.println("valor_boton = " + (String)valor_boton);
#endif

  if (valor_boton == 0)  //INPUT_PULLUP
    return 1;
  else
    return 0;
}

void actualiza_alarma(int valor_nuevo_estado) {
  //Paso de un estado a otro
  estado = valor_nuevo_estado;

  switch (estado) {
    case NORMAL:
      Serial.println("ENTRO EN ESTADO NORMAL");
      t.stop(timer_id);
      digitalWrite(PIN_LED, LOW);
      noTone(PIN_BUZZER);
      break;
    case PRE_ALARMA:
      Serial.println("ENTRO EN ESTADO PRE-ALARMA");
      tiempo_prealarma = millis();
      timer_id = t.every(1000, blink);
      break;
    case ALARMA:
      Serial.println("ENTRO EN ESTADO ALARMA");
      t.stop(timer_id);
      digitalWrite(PIN_LED, LOW);
      tone(PIN_BUZZER, NOTE_C5);
      envia_SMS(TELEFONO, "ALARMA_DE_INTRUSION");
      break;
  }
}

void blink()
{
  static boolean output = HIGH;
#if DEBUG
  Serial.println("Blink");
  output ? Serial.println("beep") : Serial.println("no beep");
#endif
  digitalWrite(PIN_LED, output);
  output ? tone(PIN_BUZZER, NOTE_C5) : noTone(PIN_BUZZER);
  output = !output;
}

void imprime_valiables() { //para debug
  delay(1000);  //Para leer por pantalla
  Serial.println("-----------------");
  Serial.print("Estado = ");
  switch (estado) {
    case NORMAL:
      Serial.println("NORMAL");
      break;
    case PRE_ALARMA:
      Serial.println("PRE_ALARMA");
      break;
    case ALARMA:
      Serial.println("ALARMA");
      break;
  }
}

boolean envia_SMS(String telefono, String mensaje) {
  String webString = "";
#if DEBUG
  Serial.println("enviando mensaje... ");
  Serial.println("connecting to server...");
#endif
  if (client.connect(server, 80)) {
#if DEBUG
    Serial.println("connected");
#endif
    client.print("GET /servicios/SMS/saveSMS.php?telefono=");
    client.print(telefono);
    client.print("&mensaje=");
    client.print(mensaje);
    client.print("&pin=");
    client.print((String)PIN);
    client.println(" HTTP/1.1");
    client.println("Host: www.aprendiendoarduino.com");
    client.println("Connection: close");
    client.println();
#if DEBUG
    Serial.print("GET /servicios/mensajes/grabaMensajes.php?telefono=");
    Serial.print(telefono);
    Serial.print("&mensaje=");
    Serial.println(mensaje);
#endif
  }
  else {
    Serial.println("connection failed");
    return 0;
  }

  while (client.available() == 0) {
    //Espero respuesta del servidor
  }

  if (client.available()) {
#if DEBUG
    Serial.println("Respuesta del Servidor---->");
#endif
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
#if DEBUG
    Serial.println(webString);
#endif
    client.stop();
    return 1;
  }
}
