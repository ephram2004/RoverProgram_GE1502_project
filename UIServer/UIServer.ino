#include <WiFi.h>               //including libraries for the code
#include "aWOT.h" 
#include "StaticFiles.h"

#define WIFI_SSID "arduino"           //defining the wifi username
#define WIFI_PASSWORD "classical"     //defining the wifi password
#define LED_BUILTIN 2                 //defining the built-in led
#define TXD0 1                        //defining the transfer (Tx) pin
#define RXD0 3                        //defining the recieve (Rx) pin
 
WiFiServer server(80);                //initialize the server
Application app;                      //launch the app to the server

bool ledOn;                           //create a boolean to turn on the led
char currentLine;                     //create a pointer to the current line recieved

////////////////////////////////////////////////////

void readLed(Request &req, Response &res) {     //make a GET request to the UI server for the LED switch
  res.print(ledOn);                            
}

void updateLed(Request &req, Response &res) {   //turn on or off the led based on the toggle button input
  ledOn = (req.read() != '0');                  
  digitalWrite(LED_BUILTIN, ledOn);
  return readLed(req, res);
}

////////////////////////////////////////////////////

void readInstructions(Request &req, Response &res) {      //to read the user instructions string from the webpage
  res.print(currentLine);
}

void updateInstructions(Request &req, Response &res) {    //to update the user instructions string from the webpage
  Serial.print('\n');                                     //prints a new line
  while(req.available())                                  //if the GET request turns an answer
        {
          //currentLine+=req.read();
          Serial.write(req.read());                       //write down the string 
        }
  //Serial.println(currentLine);
  currentLine = req.read();                               //send the string to the currentLine variable
  return readInstructions(req, res);                     
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);                           //set the LED_BUILTIN led pin as OUTPUT                           
  //Serial.begin(115200);

  Serial.begin(115200, SERIAL_8N1, RXD0, TXD0);           //initialize the serial monitor, as well as the output Tx and Rx pins

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                   //connect to the network using defined credentials
  while (WiFi.status() != WL_CONNECTED) {                 //print out a status information on connection
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());                         //when connected, display IP

  app.get("/inputText", &readInstructions);               //send GET request for the instructions
  app.put("/inputText", &updateInstructions);             //send PUT request to update instructions
  ///////////////////////////////////////////
  app.get("/led", &readLed);                              //send GET request for the led status
  app.put("/led", &updateLed);                            //send PUT request to update led state  
  //////////////////////////////////////////
  app.use(staticFiles());                                 //route the ESP32 to the website files

  server.begin();                                         //begin the server
}

void loop() {
  WiFiClient client = server.available();                 //make the server available

  if (client.connected()) {                               //if a client is connected (device)
    app.process(&client);                                 //route the correct GET and PUT statements from and to user device
  }

}
