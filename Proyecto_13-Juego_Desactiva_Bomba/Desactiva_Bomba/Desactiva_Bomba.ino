/* Juego Desactiva Bomba V1

  Función: Simula una una cuenta atras de una bomba.
  La bomba solo se puede desactivar con un código obtenido
  a través de un juego de pistas introducido en el keypad

  Reglas: Solo hay un número máximo deintentos para meter el código.
  Si fallan todos los intentos la bomba explota.

  Componentes
  - Arduino UNO
  - Displays de texto LCD 16x2
    + Tutorial: https://www.arduino.cc/en/Tutorial/HelloWorld
    + Tutorial: http://www.prometec.net/displays-lcd/
  - Matrix Keypad: Disponible en el Library Manager
    + http://playground.arduino.cc/Main/KeypadTutorial
    + Tutorial http://ticlaspalmas.com/posts/piano-con-arduino-y-un-teclado-matricial
    + Tutorial http://abedulengenharia.blogspot.com.es/2011/07/arduino-y-teclado-3x4.html
  - Buzzer: http://www.hobbytronics.co.uk/arduino-tutorial7-piezo-beepç
    + Tutorial: http://www.instructables.com/id/Arduino-YL-44-Buzzer-module/

  Librerías
  - LiquidCrystal.h: https://www.arduino.cc/en/Reference/LiquidCrystal
  - Keypad.h: https://github.com/Chris--A/Keypad

  Esquema de Pines Display LCD
  - LCD RS pin to digital pin 7
  - LCD Enable pin to digital pin 6
  - LCD D4 pin to digital pin 5
  - LCD D5 pin to digital pin 4
  - LCD D6 pin to digital pin 3
  - LCD D7 pin to digital pin 2
  - LCD R/W pin to ground
  - LCD VSS pin to ground
  - LCD VCC pin to 5V
  - pot to LCD VO pin

  Esquema de Pines teclado 4x4
  - COL0 (Pin1 keypad): A0
  - COL1 (Pin2 keypad): A1
  - COL2 (Pin3 keypad): A2
  - COL3 (Pin4 keypad): A3
  - ROW0 (Pin5 keypad): A4
  - ROW1 (Pin6 keypad): A5
  - ROW2 (Pin7 keypad): 13
  - ROW3 (Pin8 keypad): 12

  Pin Relé: 9

  Pin Buzzer: 8

*/

// define constants
#define MINUTOS 60
#define NUM_MAX_INTENTOS 3
#define NUM_DIGITOS_CLAVE 4
#define CLAVE "1234"

// include the library code:
#include <LiquidCrystal.h>
#include <Keypad.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A4, A5, 13, 12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A0, A1, A2, A3}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int intentos_restantes = NUM_MAX_INTENTOS;
int digito_clave = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(8, INPUT);  //BUZZER desactivado

  //Inicio el puerto serie
  Serial.begin(9600);
  Serial.println("Juego Desactiva Bomba V1");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Tiempo Restante");

}

void loop() {
  // put your main code here, to run repeatedly:

  int segundos_transcurridos = (millis() / 1000);
  int segundos_restantes = (MINUTOS * 60) - segundos_transcurridos;
  int minutos = segundos_restantes / 60;
  int segundos = segundos_restantes % 60;

  // set the cursor to column 5, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(5, 1);

  if (minutos < 10) {
    lcd.print("0");
    lcd.print(minutos);
  }
  else
    lcd.print(minutos);

  lcd.print(":");

  if (segundos < 10) {
    lcd.print("0");
    lcd.print(segundos);
  }
  else
    lcd.print(segundos);

  if (segundos_restantes <= 0) {
    lcd.setCursor(0, 0);
    lcd.print("BOOOMMMMM!!!!!! ");
    lcd.setCursor(0, 1);
    lcd.print("BOMBA EXPLOTADA");
    digitalWrite(9, LOW);
    for (;;) {
      pinMode(8, OUTPUT);  //BUZZER activado
    }
  }

  //leer teclado en cada LOOP
  char customKey = customKeypad.getKey();
  if (customKey) {
    if (digito_clave == 0) {  //Limpio primera linea al pulsar botón
      lcd.setCursor(0, 0);
      lcd.print("                 ");
    }
    Serial.print("Pulsado: ");
    Serial.println(customKey);
    lcd.setCursor(digito_clave, 0);
    lcd.print(customKey);
    if (customKey == CLAVE[digito_clave]) {
      digito_clave++;
      if (digito_clave == NUM_DIGITOS_CLAVE) {
        Serial.println("DESACTIVADA!!");
        lcd.setCursor(0, 0);
        lcd.print("BOMBA DESACTIVADA!!");
        for (;;) {
          //sonido intermitente
          pinMode(8, OUTPUT);  //BUZZER activado
          delay(250);
          pinMode(8, INPUT);  //BUZZER desactivado
          delay(250);
        }
      }
    }
    else {
      delay(1000);
      Serial.println("Clave incorrecta");
      lcd.setCursor(0, 0);
      lcd.print("Clave incorrecta    ");
      intentos_restantes--;
      digito_clave = 0;
      if (intentos_restantes > 0) {
        Serial.println("Te quedan: " + String(intentos_restantes) + " intentos");
        lcd.setCursor(0, 1);
        lcd.print("Quedan: " + String(intentos_restantes) + " intentos");
        delay(3000);
        lcd.setCursor(0, 0);
        lcd.print("Tiempo Restante  ");
        lcd.setCursor(0, 1);
        lcd.print("                 ");
      }
      else {
        Serial.println("BOOOMMMMM!!!!!!");
        lcd.setCursor(0, 0);
        lcd.print("BOOOMMMMM!!!!!! ");
        lcd.setCursor(0, 1);
        lcd.print("BOMBA EXPLOTADA");
        digitalWrite(9, LOW);
        for (;;) {
          pinMode(8, OUTPUT);  //BUZZER activado
        }
      }
    }
  }




  //si clave entonces sonido alternativo
  //texto bomba desactivada

  //pin 10 y 11 leds

  //tic tac con el buzzer pin8

}
