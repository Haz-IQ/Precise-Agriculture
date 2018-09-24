#include <dht.h>

dht DHT;

#define DHT11Pin 7

int TempPin = 1;
float TempVal;
float DHTTemp;
float DHTHumidity;
float LdrVal;
float MoistureVal;
int LdrPin = 0;//define a pin for Photo resistor
int MoisturePin=2;

void setup(){
  DHT.read11(DHT11Pin);
  Serial.begin(9600);
}

void loop()
{
  // Soil Temp
  TempVal = safeAnalogRead(TempPin);
  TempVal = (TempVal/1024.0)*500;

  Serial.print("SOIL TEMPRATURE = ");
  Serial.print(TempVal);
  Serial.print("*C");
  Serial.println();

  //Surrounding Temp
  Serial.print("Surrounding Temperature = ");
  DHTTemp = DHT.temperature;
  Serial.println(DHTTemp);
  Serial.print("Surrounding Humidity = ");
  DHTHumidity = DHT.humidity;
  Serial.println(DHTHumidity);

  //Photoresistor
  Serial.print("Light Intensity: ");
  LdrVal = safeAnalogRead(LdrPin);
  LdrVal = (LdrVal/1023.0)*100.0;
  Serial.print(LdrVal);
  Serial.println("%");
  
 
  
  //Soil Moisture 
  MoistureVal = safeAnalogRead(MoisturePin);
  // print out the value you read:
  Serial.print("Soil Moisture: ");
  Serial.println(MoistureVal);
  Serial.println("\n\n\n");
 
  
  delay(2000);
}


int safeAnalogRead(int pin)
{
  int x = analogRead(pin);  // make an initial reading to set up the ADC
  delay(10);                // let the ADC stabilize
  x = analogRead(pin);      // toss the first reading and take one we will keep
  delay(10);                // delay again to be friendly to future readings
  return x;
}
