#include "Pitot_library.h"
#include <SoftwareSerial.h>

//crea el objeto principal
PITOT pitotTube;

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor Inicializado :)";
  delay(1000); //esperamos un segundito para comenzar
  
  //inicia calculando el desplazamiento una vez
  //da el número de iteraciones como argumento

  pitotTube.offsetCalculation(10); // llamamos al esta función
  //calcula un desplazamiento (offset) basado en 10 iteraciones 
  //para mejorar la precisión de las mediciones.
}

void loop() {
  pitotTube.Pitot_Actualizacion(); // ejecuta desde la librería
  
  //este delay es para esperar un poco entre cada medición
  delay(500)
}
