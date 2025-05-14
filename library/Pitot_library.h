#ifndef PITOT_LIBRARY_H
#define PITOT_LIBRARY_H
#pragma once

class PITOT
{
	private:
	float measurement;
	int sensorPin;
	float offset;
	float sensorValue;
	int zero_span;
	bool signFlag;
	float Vout;
	float velocity;
	float P;
	float PinPa;
	float rho;
	
	public:
		PITOT();
		void Pitot_Actualizacion();
		void offsetCalculation(int offset_size);
		bool deadZoneCheck(float sensorValue);
		void setSensorPin(int sensorPin);
		void setZeroSpan(int zero_span);
		bool signCheck();
		void rawToVoutMap();
		void diffPressure();
		void BernoulliVelocity();
		void setRho(float rho);
	
};

PITOT :: PITOT() //esta clase nos sirve para obtener la información que necesitamos
{
	this->measurement=0;
	this->sensorPin=4;//inicializamos los valores por defecto del sensor
	this->offset=0;
	this->sensorValue=0;
	this->zero_span=2;//zona muerta
	//si el tubo de Pitot no está correctamente posicionado o si hay interferencias en el flujo, 
	//puede generarse una zona muerta donde las mediciones no sean confiables
	this->signFlag=false;
	this->Vout=0;
	this->velocity=0;
	this->P=0;
	this->PinPa=0;
	this->rho=1.204;//densidad del aire en kg/m³
	Serial.println("Muestra");
}

//Calcula el offset promedio leyendo el sensor varias veces con el aire quieto
//(sin flujo). Esto sirve para corregir errores de hardware:
void PITOT :: offsetCalculation(int offset_size){
	Serial.println("Sensor Offset Calculation");
	float sum=0,offset;

	for (int i=0;i<offset_size;i++){
		Serial.print("Offset in iteration: ");
    	Serial.println(i);
		
    	measurement=analogRead(sensorPin)-(1023/2);
  	
		Serial.println(measurement);
    	
		sum = sum+measurement;
	
    	delay(500); 
	}
	
	offset = sum / offset_size;
	Serial.println("Sensor Offset is equal to");
	Serial.println(offset);
	this->offset=offset;
}

//Este es el método principal amochis, aquí llama a todos los pasos que son
//necesarios para calcular la velocidad del aire, o sea, esto es lo mero bueno
void PITOT ::  Pitot_Actualizacion(){
	sensorValue = analogRead(sensorPin)-offset; //lee el sensor
    	deadZoneCheck(sensorValue); //verifica zona muerta
	signFlag=signCheck();
	rawToVoutMap(); //convertimos el valor a voltajes, practicamente convertimos 
					// la señal analógica que recibimos en una digital
	diffPressure(); //Calcula presión diferencial

}

//Evita usar lecturas muy cercanas a 512 (el centro), 
//que podrían ser ruido. Repite la lectura si cae en esa "zona muerta".
bool PITOT :: deadZoneCheck(float sensorValue){
	Serial.println("sensor value before dead zone check");
	Serial.println(sensorValue);
	delay(1000);
	//si se encuentra en una "zona muerta", vuelvo a leer el pin analógico
	//si no, el bucle while será ignorado
	while(sensorValue>512-zero_span and sensorValue<512+zero_span){
	sensorValue = analogRead(sensorPin)-offset;	
	Serial.println("I'm in the dead zone check");
	Serial.println("New value after dead zone check: ");
	Serial.println(sensorValue);
	delay(2000);
	}
    
	
}

void PITOT ::  setZeroSpan(int zero_span){
	this->zero_span=zero_span; //Nos ayuda a definir un margen de error en la lectura del sensor
}							//Evita fluctuaciones cerca de la zona muerta
void PITOT ::  setSensorPin(int sensorPin){
	this->sensorPin=sensorPin;
}

//Verifica si la presión fue positiva o negativa respecto al flujo:
bool PITOT ::  signCheck(){
	static bool positive;
        if (sensorValue > 512){ //Si es mayor a 512: presión positiva.
                positive=true;
                sensorValue=sensorValue-512; 
        }else{//Si es menor a 512: presión negativa.
                sensorValue=512-sensorValue;
    }
	return positive;
}

void PITOT :: rawToVoutMap(){
	Vout=5*(sensorValue)/512; //Convierte el valor leído a voltaje:
       	Serial.println("Vout is:");
       	Serial.println(Vout);
       	delay(2000);
}

//Convierte el voltaje a presión en kilopascales y después a pascales,
// luego llama a la función BernoulliVelocity
void PITOT :: diffPressure(){
	P=Vout-2.5; 
    	Serial.println("Pressure in kPa is:");
   	Serial.println(P);
    	if(P>0){
      	 	PinPa=P*1000; //in Pa
      	 	Serial.println("Pressure in Pascal");
       		Serial.println(PinPa);
	   	BernoulliVelocity();
	}
}

//Calcula la velocidad del aire usando la ecuación de Bernoulli:
void PITOT :: BernoulliVelocity(){
	velocity= sqrt(2*PinPa/rho); // Si la presión es positiva, calcula velocidad
	
        if (signFlag==false){
          	velocity=-velocity; //Si la presión es negativa, cambia el signo de la velocidad.
        }
        Serial.println(velocity); // Mostramos la velocidad calculada
       
}
void PITOT :: setRho(float rho){
	this->rho=rho;
}
#endif

