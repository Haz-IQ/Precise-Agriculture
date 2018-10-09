
//***Receiver***
//Following is the code for receiving the transmitted message (from the probe) via built-in radio transceiver SX-1278 on the Loraduino board.


/*************************************************************************************/

#include <SPI.h> //For radio communication.
#include <RH_RF95.h> //For radio communication.

/********************************************************************/ 

RH_RF95 SX1278; //radio object (SX1278 is the model number of our built-in radio transceiver).

/********************************************************************/ 


void setup()
{
    Serial.begin(115200);
    while (!Serial) ; // Wait for serial port to be available
    if (!SX1278.init()) //Initializing the radio and checking if it is initialized or not.
        Serial.println("init failed");
    //Default initialization parameters are 434.0MHz, 13dBm, Bw=125 kHz, Cr=4/5, Sf=128chips/symbol, CRC on.
}

/********************************************************************/ 

void loop()
{

/********************************************************************/ 

    if (SX1278.available()) //Checking if there is a knock knock on the receiver's door.
    { 
        // Should be a message for us now
        
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //initializing char array buf with the size of the available-to-receive message.
        uint8_t len = sizeof(buf); //checking and saving the length of the avaialable message in len.
         
        /********************************************************************/ 
        
        if (SX1278.recv(buf, &len)) //if reception is successful
        {
            Serial.println("");
            Serial.print("Receive Message: ");
            Serial.println((char*)buf); 
            Serial.println("\n\n\n");
            //Serial.print("RSSI: "); //uncomment this to also print the received signal strength.
            //Serial.println(SX1278.lastRssi(), DEC); //uncomment this to also print the received signal strength.
            Serial.write((char*)buf); //sending the received message to NodeMCU via RX TX serial lines.    
        }
        
        else //if reception is failed
        {
            Serial.println("recv failed");
        }
       
        /********************************************************************/
    }

/********************************************************************/
    
}
