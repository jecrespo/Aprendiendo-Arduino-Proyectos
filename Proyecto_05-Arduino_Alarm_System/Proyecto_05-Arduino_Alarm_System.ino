#define TIEMPO_PRE_ALARMA 10000 //Tiempo que permanece en pre-alarma

//ESTADOS
boolean alarma = 0; //Cuando está en alarma
boolean pre_alarma = 0; //Cuando detecta una condición de alarma y no ha llegado a prealarma


void setup() {


}

void loop() {
  comprueba_flanco_tilt();
  comprueba_ldr();
  estado();

}

void comprueba_flanco_tilt(){
  
}

void comprueba_ldr(){
  
}

void estado(){
  
}

