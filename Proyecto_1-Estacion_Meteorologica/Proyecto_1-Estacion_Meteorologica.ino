// Proyecto_1-Estacion_Meteorologica
// Enrique Crespo <aprendiendoarduino@gmail.com> --> https://github.com/jecrespo
// Proyecto 1 – Estación meteorológica.

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int sensorPin = A0;
const int botonPin = 6;
boolean switchState;
boolean pulsado = false;
int valorMostrado = 0;	//solo puede valer 0, 1 o 2 para normal, máxima o mínima
unsigned long tiempoPulsado;

float temperature;
float maxt = -40;
float mint = 150;

void setup()
{
  Serial.begin(9600);
  Serial.println("Iniciando...");
  lcd.begin(16, 2);
  lcd.print("Iniciando...");
  Serial.println("LCD: Iniciando...");
  pinMode(botonPin, INPUT_PULLUP);
  switchState = digitalRead(botonPin);  //Inicializo el estado del boton
}

void loop()
{
  //Leo sonda
  int sensorVal = analogRead(sensorPin);
  float voltage = (sensorVal / 1024.0) * 5.0;
  temperature = (voltage - 0.5) * 100;	//Información: http://arduino.cc/documents/datasheets/TEMP-TMP35_36_37.pdf

  //Actualizo valores max y min
  if (temperature > maxt) maxt = temperature;
  if (temperature < mint) mint = temperature;

  //Muestro por pantalla el valor actualizado de la temperatura
  if (valorMostrado == 0) {
    muestraValor(valorMostrado);
    delay(1000);  //Para evitar parpadeo en pantalla.
  }

  //Compruebo si se pulsa el botón y muestro por pantalla lo correspondiente
  if (switchState != digitalRead(botonPin)) {
    switchState = !switchState;  //actualizo el valor
    if (switchState) {					//Flanco ascendente
      Serial.println("Boton soltado, flanco ascendente.");
      if (pulsado) {
        valorMostrado ++;
        valorMostrado = valorMostrado % 3;	//Actualizo el valor mostrado entre los valores de 0 a 2
        Serial.print("Valor Mostrado: ");
        Serial.println(valorMostrado);
        muestraValor(valorMostrado);	//evito que al resetar tambien se cambie lo que sale por pantalla
      }
      pulsado = false;
    }
    else {
      Serial.println("Boton pulsado, flanco descendente.");								//Flanco descendente
      tiempoPulsado = millis();		//Guardo el tiempo en que pulso.
      pulsado = true;
    }
  }

  if (pulsado) {
    Serial.print("El botón lleva pulsado: ");
    Serial.println(millis() - tiempoPulsado);
    if (((millis() - tiempoPulsado) > 5000) && pulsado) resetValores();
  }
}


void muestraValor(int valor) {

  switch (valor) {
    case 0:
      lcd.clear();
      lcd.home();
      lcd.print("Temperatura Actual:");
      lcd.setCursor(0, 1);
      lcd.print(temperature);
      lcd.print("ºC");
      Serial.print("LCD: Temperatura Actual:");
      Serial.print(temperature);
      Serial.println(" C");
      break;
    case 1:
      lcd.clear();
      lcd.home();
      lcd.print("Temperatura Máxima:");
      lcd.setCursor(0, 1);
      lcd.print(maxt);
      lcd.print(" C");
      Serial.print("LCD: Temperatura Maxima:");
      Serial.print(maxt);
      Serial.println(" C");
      break;
    case 2:
      lcd.clear();
      lcd.home();
      lcd.print("Temperatura Mínima:");
      lcd.setCursor(0, 1);
      lcd.print(mint);
      lcd.print(" C");
      Serial.print("LCD: Temperatura Minima:");
      Serial.print(mint);
      Serial.println(" C");
      break;
  }
}

void resetValores() {
  maxt = -40;
  mint = 150;
  pulsado = false;
  lcd.clear();
  lcd.home();
  lcd.print("Valores Reseteados");
  Serial.println("LCD: Valores Reseteados");
}
