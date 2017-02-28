/*  Atrapa los 7 Fugitivos 
 *  Juego con LCD 2x16 y Arduino
 *  By: www.elprofegarcia.com  componentes comprados en: www.dinastiatecnologica.com
 *  Atrapa los fugitivos al poner tierra en el puerto analogo A5
 */
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // Configura pines de salida para el LCD
  
// defino los nuevo caracter o imagenes
byte letra1[8] = { B01110, B01110, B01110, B00100, B11111, B00100, B01010, B01010 }; // fugitivo
byte letra2[8] = { B01110, B01110, B01110, B10101, B01110, B00100, B01010, B10001 }; 

byte letra3[8] = { B00100, B00100, B00100, B00100, B00100, B11111, B01110, B00100 }; //flecha

byte letra0[8] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000 }; // blanco
byte letra4[8] = { B01110, B01110, B01110, B11111, B10101, B01110, B01010, B11011 }; // atrapado

int x=0;
int y=1;
int retardo=200;
int presos=0;
int libres=7;
int fallas=0;
int puntaje=0;

void setup() {
  lcd.begin(16, 2);               // Configura el LCD con 16 columnas y 2 filas
  lcd.createChar(1, letra1);      // asigna los nuevos caracteres por numeros
  lcd.createChar(2, letra2);
  lcd.createChar(3, letra3);
  lcd.createChar(0, letra0);
  lcd.createChar(4, letra4);
} 

void loop() {
 
  lcd.setCursor(0, 0);   // Posicion del letrero en 0,0
  lcd.print(presos); 
  lcd.setCursor(2, 0);   // Posicion del letrero en 2,0
  lcd.print("F"); 
  lcd.setCursor(3, 0);   // Posicion del letrero en 3,0
  lcd.print(fallas); 
  lcd.setCursor(0, 1);   // Posicion del letrero en 0,1
  lcd.print(libres); 

  lcd.setCursor(x,y);  // imagen fugitivo 1
  lcd.write(byte(1)); 
  delay(retardo);

  lcd.setCursor(x,y);  // imagen fugitivo 2
  lcd.write(byte(2));
  delay(retardo);

  lcd.clear();        // borrar Pantalla

  lcd.setCursor(14,0); // Flecha en posicion 
  lcd.write(byte(3));
  
  lcd.setCursor(5, 0);   // Posicion del letrero
  lcd.print("Atrapelo"); 
  
  if (analogRead(A5)<= 10) {  // Si es oprimido el interruptor A5
    lcd.setCursor(14,0);       // Se borra la flecha
    lcd.write(byte(0));
    lcd.setCursor(14,1);      // Baja la flecha
    lcd.write(byte(3));
   
    if (x == 14) {            // Si coincide en la posicion 14 y es capturado
       lcd.setCursor(5, 0);
       lcd.print("Bien   ."); 
       lcd.setCursor(x,y);    // Coloca imagen de capturado
       lcd.write(byte(4));
       presos++;   
       libres--;  
    }
    else{                        // Cuando no es capturado
       lcd.setCursor(5, 0);
       lcd.print("Fallo  ."); 
       lcd.setCursor(x,y);       // donde quedo el corredor, Imagen de que sigue libre 
       lcd.write(byte(2));
       fallas++;
    }
  delay(3000);                     // Espera 3 segundos
  }
 
  x=x+1;                         // avanza el corredor
  
  if (x>=16) {
     x=0;                        // al pasar regresa al inicio
  }  
 
  retardo = random(1,150);       // Escoge un retardo aleatorio

 if (fallas == 3) {                 // Si coincide en la posicion 14 y es capturado
       lcd.clear();                 // borrar Pantalla
       lcd.setCursor(3, 0);
       lcd.print("NO PUDO ..."); 
       puntaje=presos-fallas;
       lcd.setCursor(3, 1);
       lcd.print("Puntaje:"); 
       lcd.setCursor(12, 1);
       lcd.print(puntaje); 
       delay (7000);
       presos=0;
       libres=7;
       fallas=0;
       lcd.clear();        // borrar Pantalla
    }
 if (libres == 0) {             // Si coincide en la posicion 14 y es capturado
       lcd.clear();             // borrar Pantalla
       lcd.setCursor(3, 0);
       if (fallas == 0) {              // Si no hubo fallas
         lcd.print("EXCELENTE!!!!"); 
       }
       else{
         lcd.print("PUEDE MEJORAR");    // si termino pero con alguna falla
       }
       
       puntaje=presos-fallas;
       lcd.setCursor(3, 1);
       lcd.print("Puntaje:"); 
       lcd.setCursor(12, 1);
       lcd.print(puntaje);               // Puntaje final 
       delay (7000);
       presos=0;
       libres=7;
       fallas=0;
       lcd.clear();        // borrar Pantalla
    }   
}
