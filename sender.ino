#include <dht.h>
#include <SPI.h>
#include <RH_RF95.h>
//#include <ArduinoJson.h>
RH_RF95 SX1278;
dht DHT;


#define DHT11Pin 7
//StaticJsonBuffer<200> jsonBuffer;
//JsonObject& root = jsonBuffer.createObject();
char payload[150];
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
  while (!Serial) ; // Wait for serial port to be available
   
    if (!SX1278.init())
        Serial.println("Notice:init failed");
    
    // Defaults init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
}

void loop()
{
  // Soil Temp
  TempVal = safeAnalogRead(TempPin);
  TempVal = (TempVal/1024.0)*500*0.72;

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
  MoistureVal = (MoistureVal/1023.0)*100.0;
  // print out the value you read:
  Serial.print("Soil Moisture: ");
  Serial.println(MoistureVal);
  Serial.println("\n\n\n");
  /*char tempval[3];
  char dhttemp[3];
  char dhthumidity[3];
  char moistureval[4];
  char ldrval[3];
  itoa ((int)TempVal,tempval,10);
  itoa ((int)DHTTemp,dhttemp,10);
  itoa ((int)DHTHumidity,dhthumidity,10);
  itoa ((int)MoistureVal,moistureval,10);
  itoa ((int)LdrVal,ldrval,10);

  char testing[5] = {'0','0','0','C','C'};
  testing[0]=tempval[0];
  testing[1]=tempval[1];
  testing[2]=tempval[2];

  Serial.println(testing);

  */
  String s1 = "<{\"tempsoil\" : " + (String)((int)TempVal) + ", \"tempair\" : " + (String)((int)DHTTemp) + ", \"humidity\" : " + (String)((int)DHTHumidity) + ", \"moisture\" : " + (String)((int)MoistureVal) + ", \"ldrval\" : " + (String)((int)LdrVal)+"}>";
  s1.toCharArray(payload,150);

  /*root["tempsoil"] = (int)TempVal;
  root["humidity"] = (int)DHTHumidity;
  root["tempair"] = (int)DHTTemp;
  root["ldrval"] = (int)LdrVal;
  root["moisture"] = (int)MoistureVal;

  root.printTo(Serial);
  Serial.println();
  root.printTo((char*)payload, root.measureLength() + 1);
  */
  // Send a message to SX1278_server
  Serial.println(payload);
  uint8_t lennn = sizeof(payload);
  SX1278.send(payload, lennn);
  Serial.println("mark0");
  SX1278.waitPacketSent();
  Serial.println("mark1");
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  delay(1000);
  Serial.println("mark2"); 
  
  
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
