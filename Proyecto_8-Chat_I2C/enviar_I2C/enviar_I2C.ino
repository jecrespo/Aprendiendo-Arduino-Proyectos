#include <Wire.h>

void setup()
{
  Wire.begin();	//Iniciamos el bus I2C
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available()) {
    while (Serial.available()) {
      byte outChar = (byte)Serial.read();
      Wire.beginTransmission(0xF5); // Enviamos a la dirección 0xF5 en binario 111 0101
      Wire.write(outChar);          // Enviamos el byte leido por el puerto serie
      Wire.endTransmission();    // Finaliza la transmisión al esclavo y transmite los bytes encolados
      delay(5);
    }
  }

  delay(100);
}
