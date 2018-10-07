#include <ArduinoJson.h>

/*
  MySQL Connector/Arduino Example : connect by wifi

  This example demonstrates how to connect to a MySQL server from an
  Arduino using an Arduino-compatible Wifi shield. Note that "compatible"
  means it must conform to the Ethernet class library or be a derivative
  thereof. See the documentation located in the /docs folder for more
ssible from the network or the user name and  details.
 password is incorrect.

  Created by: Dr. Charles A. Bell
*/
#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.N
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server_addr(192,168,0,35);  // IP of the MySQL *server* here
int tt=0;
int relaypin = 16;
const byte numChars = 150; 
char receivedChars[numChars];
bool newData = false;
char user[] = "node";              // MySQL user login username
char password[] = "123";        // MySQL user login password
char buf[150];
// Sample query
char INSERT_SQL[] = "INSERT INTO dcc.pa (Id,Time, Temp_soil, Temp_atm, Humidity, Moisture, Light) VALUES ('',%d,%d,%d,%d,%d,%d)";

// WiFi card example
char ssid[] = "TP-LINK_CE64";         // your SSID
char pass[] = "dccinterns18";     // your SSID Password

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;


void setup()
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
    cursor = new MySQL_Cursor(&conn);
  pinMode(relaypin,OUTPUT);
  digitalWrite(relaypin,LOW);

}

void loop()
{
   recvWithStartEndMarkers();
   showNewData();
   //if(newData == true)
   
   StaticJsonBuffer<200> jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject(receivedChars);
   if(!root.success()){
  Serial.println("parseObject() failed");
  return;
  }
  int tempsoil=root["tempsoil"];
  int tempair=root["tempair"];
  int humidity=root["humidity"];
  int moisture=root["moisture"];
  int ldrval=root["ldrval"];

  if (moisture<40)

  { digitalWrite(relaypin,LOW);
    delay(2000);
    digitalWrite(relaypin,HIGH);
  }
    
  char query[150];
  sprintf(query, INSERT_SQL, tt,tempsoil,tempair,humidity,moisture,ldrval);
  
  //sprintf(query, INSERT_SQL, tt,32,33,34,35,36);
  
 // MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);*/
  
  if (conn.connected())
    //cursor->execute("INSERT INTO dcc.test2 (Time, Tempsoil, Tempair, Humidity, Moisture, Light) VALUES (11,22,33,44,55,66)");
 // delete cur_mem;
 cursor->execute(query);
  Serial.println("Data recorded.");
  tt +=1;
  if(tt==24)tt=0;
  
  delay(5000);
  //newData=false;
  
  
}

void recvWithStartEndMarkers() {
   
    static bool recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
         //Serial.println("into the recv function...");
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.println("Received Serially: ");
        Serial.println(receivedChars);
        newData = false;
    }
}
