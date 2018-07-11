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

int test1 = 8;
int test2 = 9;

String test_array[2] = {String(test1), String(test2)};

int status = WL_IDLE_STATUS;     // the WiFi radio's status

IPAddress server(10,0,0,1);  //This is the address of the MQTT server to be used
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);     // This section of code creates an object

void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // THE FOLLOWING CODE CAUSES ERROR, NOT SURE WHY
  // check for the presence of the shield:
  //if (WiFi.status() == WL_NO_SHIELD) {
   // Serial.println("WiFi shield not present");
    // don't continue:
   // while (true);
  //}

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

void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
    if (!mqttclient.connected()){
    Serial.println("Disconnected...");
    reconnect();   // If server disconnect happens, reconnect
  }
  else if(mqttclient.connected()){
    Serial.println("Still Connected...");
    // publish data every 10 seconds
    if (millis() > (time + 10000)){
      time = millis();
      float temp = 1584; // test value. actual code would be dht.getTemperature() or something
      pubString = String(temp);  // Converts from float to string
      pubString.toCharArray(tem, temp_str.length() + 1); // Packages data to be sent through MQTT
      mqttclient.publish("Temp",tem);  // If server is connected, publish data 
    }
  }  
  mqttclient.loop();  // Loop forever
}

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

