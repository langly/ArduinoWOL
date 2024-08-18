/*
 * Small program to handle interrupt from a given pin, and send a WoL packet using UDP to a machine in order to wake it up.
 */
#include <Arduino.h>
#include <WiFi.h>

// Configuration Parameters

// At which pin are we looking for the interrupt 
const byte interruptPin = 12;

// The mac address we are sending to.
byte mac_addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

// Network name and password.
const String wifi_name = "wifi-name";
const String wifi_pass = "wifi-password";


/*************************
* Actual code starts here.
**************************/

byte magic_pkt[6+(16*6)];
IPAddress broadcast;
bool wake_up;

void wakeUpComputer(){
  // Set up UDP connection here. 
  // Send the magic packet to the network broadcast address. 

  Serial.println("Trying to wake up computer!");
  WiFiUDP udp;

  // For some reason this library dies if we try
  // to send UDP packet on port 0 ( Not valid port, but still)
  int i = udp.beginPacket(broadcast, 1);

  if (i){
    for (int i = 0; i < sizeof(magic_pkt); i++){
        udp.write(magic_pkt[i]);
    }
    udp.endPacket();
  } else {
    Serial.println("Could not begin UDP");
  }
}

void initWifi(){
  WiFi.begin(wifi_name, wifi_pass);

  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Waiting for WiFI");
    delay(500);
  }

  // Seems like IPAddress in the WiFi library is a simple integer 
  // with some convenient functions to print it out. 
  IPAddress ip = WiFi.localIP();
  IPAddress mask = WiFi.subnetMask();
  IPAddress network = ip & mask; // This is just for me to debug.
  
  // Calculate the broadcast address here; I.e. highest address of the network 
  broadcast = (ip&mask) | (~0 & ~mask);

  Serial.println(ip);
  Serial.println(mask);
  Serial.println(network);
  Serial.println(broadcast);

  // Create the magic packet here;
  memset(magic_pkt, 0xFF, 6);

  byte *c = magic_pkt + 6; 
  for ( int i = 0 ; i < 16; i++){
    memcpy(c, mac_addr, 6*sizeof(char));
    c += 6;
  }
}

void shipIt(){
  // The problem is that the Serial communication takes some extra time, and that makes the interrupt watchdog die horribly.
  // So, to solve the problem we flag it here, and then we check in the loop if the flag is set.. 
  wake_up = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  wake_up = false;

  while(!Serial){
    // Required for native USB port only.
  }

  pinMode(LED_BUILTIN, OUTPUT);
  initWifi(); 

  // On Rising edge of interrupt pin, call shipIp
  attachInterrupt(digitalPinToInterrupt(interruptPin), shipIt, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, LOW);
  
  if ( wake_up ){
    wakeUpComputer(); 
    wake_up = false;
  }
  
}