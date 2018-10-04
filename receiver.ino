#include <SPI.h>
#include <RH_RF95.h>
//#include <ArduinoJson.h> 

// Singleton instance of the radio driver
RH_RF95 SX1278;

void setup()
{
    Serial.begin(115200);
    while (!Serial) ; // Wait for serial port to be available
    if (!SX1278.init())
        Serial.println("init failed");
    // Defaults init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
}

void loop()
{
    if (SX1278.available())
    {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (SX1278.recv(buf, &len))
        {
            //RH_RF95::printBuffer("request: ", buf, len);
            Serial.println("");
            Serial.print("Receive Message: ");
            Serial.println((char*)buf);
            Serial.println("\n\n\n");
            //Serial.print("RSSI: ");
            //Serial.println(SX1278.lastRssi(), DEC);
            Serial.write((char*)buf);
            /*StaticJsonBuffer<200> jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(buf);
            if (!root.success()) {
            Serial.println("parseObject() failed");
              return;}
            int tempsoil = root["tempsoil"];
            int tempair = root["tempair"]; 
            int humidity = root["humidity"]; 
            int moisture = root["moisture"]; 
            int ldrval = root["ldrval"]; 
            Serial.println(tempsoil);
            Serial.println(humidity);
            Serial.println(tempair);
            Serial.println(ldrval);
            Serial.println(moisture);
              */       
        }
        else
        {
            Serial.println("recv failed");
        }
    }
}
