/*

 This code connects to an unencrypted WiFi network
 hosted on a Raspberry Pi named Pi_AP. This Pi's sole
 purpose is to provide a connection between the Arduino
 Sensor Pod and the destination Raspberry Pi.
 
 Then it prints the  MAC address of the WiFi shield,
 the IP address obtained, and other network details.

 Then it connects to the MQTT server on the destination
 Raspberry Pi (IP 10.0.0.1) and publishes to a topic.

 Circuit:
 * WiFi shield attached

 Original Example created 13 July 2010
        by dlf (Metodo2 srl)
 Sensor Code added 9 July 2018
        by Samantha Freitas
 */
#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>


#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

long lastMsg = 0; // keeps track of how many messages
int value = 0;

// Variables for printing temp data. Replace with variables for sensor data!!!

String tempString; //see code block below use these to convert the float that you get back from DHT to a string =str
char tem[50]; // char array for how many characters can be sent in the message.
              // For example, tem (this temporary data) can be a max of 50 characters in the
              // float. 

int status = WL_IDLE_STATUS;     // the WiFi radio's status

IPAddress server(10,0,0,1);  //This is the address of the MQTT server to be used
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);     // This section of code creates an object

/* This block of code initializes the Wifi connection
 *  and the MQTT server connection. It also opens the 
 *  serial port.
 */
 
void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWiFiData();
  
  //connect to MQTT sever
  mqttclient.setServer(server, 1883);
 
}

/* This block of code is for looping through and making sure that
 *  the Arduino is still connected to both the internet and the
 *  MQTT server. 
 *  
 *  Error handling is present in case of disconnect.
 */
 
void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();   // This is for testing connection for underwater wifi
    if (!mqttclient.connected()){
    Serial.println("Disconnected...");
    reconnect();   // If server disconnect happens, reconnect
  }
  else if(mqttclient.connected()){
    Serial.println("Still Connected...");
    // publish data every 10 seconds

/* This block of code is specifically for the publishing of the sensor data. Some lines
 *  in here will be edited for this. 
 *  
 *  For example, float temp will become .getTemperature() or something similar.
 *  tempString will become humString if it is for Humidity. etc.
 */
 
//counter for the messages, see if I am missing any on the Mqtt broker
  
  long now = millis(); 
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    
//Preparing for mqtt send
      float temp = 1584; // test value. actual code would be dht.getTemperature() or something
      tempString = String(temp);  // Converts from float to string
      tempString.toCharArray(tem, tempString.length() + 1); // Packages data to be sent through MQTT
      mqttclient.publish("Temp",tem);  // If server is connected, publish data 
    }
  }  
  mqttclient.loop();  // Loop forever
}

/* This block of code is for reconnecting in case of
 *  any disconnection from the MQTT client.
 */
 
void reconnect(){
  // loop until we're reconnected
  while (!mqttclient.connected()){
    Serial.print("Attempting MQTT connection...");
    // attempt to connect
    if (mqttclient.connect("Pi_AP")){
      Serial.println("connected");
    } 
    else{
      Serial.print("failed, rc=");   // rc=-2 means no network connection
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/* This block of code is for printing Wifi data.
 */
void printWiFiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

/* This block of code is for printing network data.
 */
 
void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

