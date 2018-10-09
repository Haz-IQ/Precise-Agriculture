
//***Uplink***
//The following code is used to upload the field-data to the database via NodeMCU esp8266 Wi-Fi module.

/*****************************************************************/

#include <ESP8266WiFi.h> //NodeMCU has esp8266 wifi module built-in.
#include <MySQL_Connection.h> //to connect to the mysql database and perform operations like insertion etc.
#include <MySQL_Cursor.h> //to connect to the mysql database and perform operations like insertion etc.
#include <ArduinoJson.h> //in order to parse the received message and extract integer values from it.

/*****************************************************************/

int relaypin = 16; //relay is connected to D0 of the NodeMCU which is equivalent to pin 16 with respect to Arduino IDE.

/*****************************************************************/

char ssid[] = "TP-LINK_CE64";         // your Wi-Fi SSID
char pass[] = "dccinterns18";     // your Wi-Fi Password
IPAddress server_addr(192,168,0,35);  // IP of the MySQL server here
char user[] = "node";              // MySQL user login username
char password[] = "123";        // MySQL user login password

/*****************************************************************/

int tt=0; //a variable which will increment by '1' and reset to '0' after '23' to represent/simulate 'hours' i.e. 0-23.
const byte numChars = 150; //a safe maximum lenth of our received message via RX TX Serial communication between gateway/receiver and uplink.
char receivedChars[numChars]; //array initialization for received message.
bool newData = false; //variable to keep check of new incoming data.

/*****************************************************************/

//mysql query for insertion where %d will later be replaced by parsed integer values.
//here database name is "dcc", table name is "pa", column names are "Id","Time","Temp_soil","Temp_atm","Humidity","Moisture","Light".
char INSERT_SQL[] = "INSERT INTO dcc.pa (Id,Time, Temp_soil, Temp_atm, Humidity, Moisture, Light) VALUES ('',%d,%d,%d,%d,%d,%d)";

/*****************************************************************/

WiFiClient client;              //creating an instance for Wi-Fi Connection.
MySQL_Connection conn(&client); //passing "client" as a reference for mysql connection.
MySQL_Cursor* cursor;           //this pointer object is used for data insertion operation into mysql.

/*****************************************************************/


void setup()
{ 
  
  Serial.begin(115200);
  
  pinMode(relaypin,OUTPUT);
  digitalWrite(relaypin,HIGH); //turning the relay "off". the relay board turns "on" when written "LOW". 
  
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only
  
  /*****************************************************************/
    
  //Wi-Fi connection attempt
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED){ //printing dots till the connection is established.   
    delay(500); 
    Serial.print(".");}
  //Printing out info about the Wi-Fi connection
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP()); //prints the IP Address of the NodeMCU.
  
  /*****************************************************************/

  //MySQL connection attempt
  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password)) //here 3306 is the port of Mysql database on our server.
    Serial.println("OK."); //successfully connected to mysql database.
  else
    Serial.println("FAILED."); //connection to mysql database failed.  
  cursor = new MySQL_Cursor(&conn); //assigning the cursor to the database for insertion operation.
  
  /*****************************************************************/

}

void loop()
{
 
  /*****************************************************************/

   //receiving data serially via rx tx lines from the gateway/receiver Loraduino,
   recvWithStartEndMarkers(); //function defined at the end of the code
   showNewData(); //function defined at the end of the code
   
  /*****************************************************************/

  //Parsing the received JSON format char array/string message to extract the sensor data from it in the integer form.
   StaticJsonBuffer<200> jsonBuffer; //200 is a safe size for our message extraction variable (we can calculate it from online JSON assistant).
   JsonObject& root = jsonBuffer.parseObject(receivedChars); //received char contains the serially received message.
   if(!root.success()){
  Serial.println("parseObject() failed"); //parsing fails when message is empty or isn't in standard JSON format.
  return; //restarts loop()
  }
  //now extracting and saving the received values in newly created variables.
  int tempsoil=root["tempsoil"];
  int tempair=root["tempair"];
  int humidity=root["humidity"];
  int moisture=root["moisture"];
  int ldrval=root["ldrval"];

  /*****************************************************************/

  //Turning the relay "ON" if the soil moisture drops below a threshold.
  if (moisture<40) //here 40 is the threshold.
  { digitalWrite(relaypin,LOW);
    delay(2000); //relay will be "ON" for 2 seconds before turning "OFF" again.
    digitalWrite(relaypin,HIGH);
  }

  /*****************************************************************/

  //preparing the final mysql insertion command. 
  char query[150]; //a new char array for the modified mysql query.
  //now replacing "%d"s in the INSERT_SQL[] array with variable values like tt,tempsoil... etc. and storing it in "query".
  sprintf(query, INSERT_SQL, tt,tempsoil,tempair,humidity,moisture,ldrval); 
  
  /*****************************************************************/

  //Running our insertion query which stores the sensor values in the database.
  if (conn.connected()) //if mysql is still connected.
  cursor->execute(query); //run the insertion command in "query".
  Serial.println("Data recorded."); //data recorded in the database.
  
  /*****************************************************************/

  //tt is incremented in each loop and reset after 23 thus acting like an hour counter which is fed to the database.
  tt +=1;
  if(tt==24)tt=0;

  /*****************************************************************/

  delay(5000); //waiting 5 seconds before another insertion in the database.
  
  
}

/*****************************************************************/

//a function to receive data from RX TX serial communication between the receiver Loraduino and NodeMCU.
//it ensures synchronization by detecting the start and end markers in the message and only saving the message that is in between the markers.
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

/*****************************************************************/

//a function to show the newly received data 
void showNewData() {
    if (newData == true) {
        Serial.println("Received Serially: ");
        Serial.println(receivedChars);
        newData = false;
    }
}

/****************************************************************************************************/

