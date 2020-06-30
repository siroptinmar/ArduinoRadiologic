
/*
    Make an OSC message and send it over UDP
    
    Adrian Freed
 */
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCMessage.h>

EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(192, 168, 1, 102);

//destination IP
IPAddress outIp(192, 168, 1, 101);

//ports numbers
const unsigned int inPort = 12347; /// from Rasp <- Python
const unsigned int outPort = 12344; ///to Rasp -> Python

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // you can find this written on the board of some Arduino Ethernets or shields

int ledPin = 4;
int button1 = 3; 
int val = 0;
int oldVal = 0;
int tag = 0;
OSCErrorCode error;

///control the Led from OSC
void led (OSCMessage &m){
    if (m.getInt(0) == 0){
      digitalWrite(ledPin, HIGH);
      tag = 0;
    }
    else {
      digitalWrite(ledPin, LOW);
      tag = 1;
    }
}

void setup() {
    Ethernet.begin(mac,ip);
    Udp.begin(inPort);
    //Serial.begin(115200);
    pinMode(3,INPUT_PULLUP);
    pinMode(4,OUTPUT);
}


void loop(){

    /// dispatch OSC IN
    
    OSCMessage msg;
    int size = Udp.parsePacket();
    if (size > 0) {
      while (size--) {
        msg.fill(Udp.read());
      }
      if (!msg.hasError()) {
        msg.dispatch("/interrupteur/services", led);
      } else {
        error = msg.getError();
        //Serial.print("error: ");
        //Serial.println(error);
      }
    }
  
   
    /// gestion bouton/led envoie OSC vers rasp
    
    val = digitalRead(button1); ///val = 1 par default
    if (val !=oldVal){
      if (val == 0){
        if( tag ==0){ ///si c'est eteint: j'allume
          sendService();
          digitalWrite(ledPin, LOW);
          tag = 1;
        }
        else if (tag==1){ ///si c'est allumé: j'éteins
          sendOff();
          digitalWrite(ledPin, HIGH);
          tag = 0;
        }
      }
      else if (val == 1){
        /// si je relache le bouton : rien
      }
    }
    oldVal = val; 
    
}

void sendService(){
    ///Envoie memoire
    OSCMessage msg("/interrupteur/services");
    msg.add(1);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msg.empty(); // free space occupied by message
    delay(20);
    
    ///envoie master ??

}

void sendOff(){
    OSCMessage msg("/interrupteur/services");
    msg.add(0);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msg.empty(); // free space occupied by message
    delay(20);
  
    ///envoie master ??
  
}
