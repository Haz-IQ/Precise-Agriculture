
//***Probe***
//Following is the code for Data sensing and transmitting it to the Hub via built-in radio transceiver SX-1278 on the Loraduino board


/************************************************************************************************************/

#include <dht.h> //For DHT-11 Sensor.
#include <OneWire.h> //For 18B20 sensor.
#include <DallasTemperature.h> //For 18B20 sensor.
#include <SPI.h> //For radio communication.
#include <RH_RF95.h> //For radio communication.

/********************************************************************/ 

#define ONE_WIRE_BUS 9 //18B20 data wire is plugged into digital pin 9 on the Loraduino.
#define DHT11Pin 7 //DHT-11 data wire is plugged into digital pin 7 on the Loraduino.
int LdrPin = 0; //Photo resistor on analog 0 of Loraduino.
int MoisturePin=3; //Moisture sensor on analog 3 of Loraduino.

/********************************************************************/

dht DHT; //dht object for dht-11 sensor.
OneWire oneWire(ONE_WIRE_BUS); //Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs).
DallasTemperature sensors(&oneWire); //Passing our oneWire reference to Dallas Temperature. 
RH_RF95 SX1278; //radio object (SX1278 is the model number of our built-in radio transceiver).

/********************************************************************/

char payload[150]; //a char array in which the message to be sent (via radio) will be stored.
float TempVal; //Soil temp var.
float DHTTemp; //Surrounding temp var.
float DHTHumidity; //Surrounding humidity var.
float LdrVal; //Light intensity var.
float MoistureVal; //Moisture value var.

/********************************************************************/

void setup(){
  
  DHT.read11(DHT11Pin); //function to start listening to dht11 input.
  Serial.begin(9600);
  sensors.begin(); //function to start listening to 18B20 sensor.
  
  while (!Serial) ; // Wait for serial port to be available.
   
    if (!SX1278.init()) //Initializing the radio and checking if it is initialized or not.
        Serial.println("Notice:init failed");
    //Default initialization parameters are 434.0MHz, 13dBm, Bw=125 kHz, Cr=4/5, Sf=128chips/symbol, CRC on.
}

void loop()
{
  
  /********************************************************************/
  
  //Reading Soil Temp by 18B20 sensor
  sensors.requestTemperatures(); //to read the temperature provided as input.
  Serial.print("Soil Temperature= "); 
  TempVal= sensors.getTempCByIndex(0); //there can be multiple sensors with multiple indices, in our case there is only one at default index 0.
  Serial.println(TempVal);
  
  /********************************************************************/

  //Again initializing the radio because after calling "sensors.requestTemperatures()" in the above code, radio stops working and doesn't transmit final message.  
  if (!SX1278.init())
        Serial.println("Notice:init failed");
  
  /********************************************************************/
    
  //Reading Surrounding Temp and Humidity by DHT-11 sensor.
  Serial.print("Surrounding Temperature = ");
  DHTTemp = DHT.temperature; //Temp in celsius stored in our variable.
  Serial.println(DHTTemp);
  Serial.print("Surrounding Humidity = ");
  DHTHumidity = DHT.humidity; //Humidity in percentage stored in our variable.
  Serial.println(DHTHumidity);

  /********************************************************************/
  
  //Reading Light Intensity by Photo resistor
  Serial.print("Light Intensity: ");
  LdrVal = safeAnalogRead(LdrPin); //Analog reading stored in LdrVal. The function "safeAnalogRead" is defined at the end of the code. 
  LdrVal = (LdrVal/1023.0)*100.0; //Converting analog reading (0-1023) to percentage.
  Serial.print(LdrVal);
  Serial.println("%");
  
  /********************************************************************/
  
  //Reading Soil Moisture by analog soil moisture sensor
  MoistureVal = safeAnalogRead(MoisturePin); //analog reading is stored in our variable.
  MoistureVal = 100-((MoistureVal/1023.0)*100.0); //converting the moisture to percentage. (also subtracting it from 100 so that "0" means "dry" and "100" means "wet".
  Serial.print("Soil Moisture: ");
  Serial.println(MoistureVal);
  Serial.println("\n\n\n");

  /********************************************************************/

  //creating a "String" which contains all the measured values.
  //the String has angle brackets at start and end which act as start and end markers for the message. 
  //The markers are required for synchronized wired serial communication between the receiver Loraduino and the NodeMCU.
  //If the brackets are removed the string looks exactly like JSON format, this helps in parsing the string and extracting the integer values from it after reception.
  String s1 = "<{\"tempsoil\" : " + (String)((int)TempVal) + ", \"tempair\" : " + (String)((int)DHTTemp) + ", \"humidity\" : " + (String)((int)DHTHumidity) + ", \"moisture\" : " + (String)((int)MoistureVal) + ", \"ldrval\" : " + (String)((int)LdrVal)+"}>";
  s1.toCharArray(payload,150); //Converting the string to a char array (because the radio can't send "String" but can send a char array) and saving it in payload.
  
  /********************************************************************/
  
  //Sending the message to the receiver Loraduino.
  Serial.println(payload);
  uint8_t lennn = sizeof(payload); //saving the size of the message in lennn (uint8_t (i.e. char) data type is used because it is required as parameter in the .send() function).
  SX1278.send(payload, lennn); //transmitting the message.
  Serial.println("mark0"); //The message is loaded and ready to be transmitted.
  SX1278.waitPacketSent(); //Wait till complete transmission of the message.
  Serial.println("mark1"); //The message is transmitted and is on air.
  // Now wait for a reply (we are not receiving an acknowledgement here so the following four lines can be ignored).
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  delay(1000);
  Serial.println("mark2"); //After receiving the empty acknowledgement. 
  
  /********************************************************************/
  
  delay(2000); //2 seconds delay before again starting the sensing process. 
  
}



/********************************************************************/

//A function is created to avoid reading false analog values which are generated because we are using multiple analog pins here on the Loraduino.
//When we are reading any analog input pin on the Loraduino and we switch to another pin to read the other sensor, the first reading that we get from the second pin is false.
//In order to avoid this unstability we take 2 readings from the sensor, the first is false so we overwrite it with the second reading.

int safeAnalogRead(int pin)
{
  int x = analogRead(pin);  //Make an initial reading to set up the ADC
  delay(10);                //Let the ADC stabilize
  x = analogRead(pin);      //Toss the first reading and take one we will keep
  delay(10);                //Delay again to be friendly to future readings
  return x;
}

/**********************************************************************************************************/
