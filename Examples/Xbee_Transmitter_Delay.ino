#include <xbee_Arduino.h>
#include "SoftwareSerial.h"

xbee xbeeSerial;
String response;
// 64-bit destination address for unicast trasmittion
char destination_address[8] =  {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB5, 0x8E, 0xAC};
// 16-bit destination address for unicast trasmittion
char destination_address16[2] = {0x6B, 0x19}; 
// Number of relay nodes
int numHops = 3;
// Relay nodes' 16-bit addresses
char route[6] = {0xB7, 0x4C, 0xE3, 0xE4, 0xAF, 0x9B};

// Maximum application layer packet size
int maxPacketSize[4] = {84, 80, 78, 76};
// delay between adjacent packets in ms
long delay_between_packets[4] = {0, 13, 20, 28};
int packetId = 1;
// Application layer payload size
int payloadSize = 200;
int remainingSize;

// Whether ACKs are enabled
bool enable_acks = false;

int packetCounter = 0;

SoftwareSerial xbee_Serial(2, 3);

void setup() {
  xbeeSerial.begin(115200);
  Serial.begin(115200);
  xbee_Serial.begin(115200);
  Serial.println("online");
  xbeeSerial.create_route(destination_address, destination_address16, numHops, route);
  delay(5000);
}

void loop() {
  delay(1000);
  char buff[300];
  bool packet_sent = false;
  remainingSize = payloadSize;
  while (!packet_sent) {
    delay(delay_between_packets[numHops]);
    if (payloadSize <= maxPacketSize[numHops]) {
      packetCounter++;
      Serial.println(packetCounter);
      xbeeSerial.getFrame(buff, '0', payloadSize, '\n', enable_acks);
      for (int i = 0; i < payloadSize + 18; i++) {
        xbee_Serial.write(buff[i] & 0xFF);
      }
      packet_sent = true;
    } else {
      if (packetId == 1) {
        packetCounter++;
        Serial.println(packetCounter);
        xbeeSerial.getFrame(buff, '1', maxPacketSize[numHops], '-', enable_acks);
        for (int i = 0; i < maxPacketSize[numHops] + 18; i++) {
          xbee_Serial.write(buff[i] & 0xFF);
        }
        remainingSize -= maxPacketSize[numHops];
        packetId = 2;
      } else if (packetId == 2) {
        if (remainingSize <= maxPacketSize[numHops]) {
          xbeeSerial.getFrame(buff, '2', remainingSize, '\n', enable_acks);
          for (int i = 0; i < remainingSize + 18; i++) {
            xbee_Serial.write(buff[i] & 0xFF);
          }
          packetId = 1;
          packet_sent = true;
        } else {
          xbeeSerial.getFrame(buff, '2', maxPacketSize[numHops], '-', enable_acks);
          for (int i = 0; i < maxPacketSize[numHops] + 18; i++) {
            xbee_Serial.write(buff[i] & 0xFF);
          }
          packetId = 3;
          remainingSize -= maxPacketSize[numHops];
        }
      } else if (packetId == 3) {
        if (remainingSize <= maxPacketSize[numHops]) {
          xbeeSerial.getFrame(buff, '3', remainingSize, '\n', enable_acks);
          for (int i = 0; i < remainingSize + 18; i++) {
            xbee_Serial.write(buff[i] & 0xFF);
          }
          packetId = 1;
          packet_sent = true;
        } else {
          xbeeSerial.getFrame(buff, '4', maxPacketSize[numHops], '-', enable_acks);
          for (int i = 0; i < maxPacketSize[numHops] + 18; i++) {
            xbee_Serial.write(buff[i] & 0xFF);
          }
          packetId = 4;
          remainingSize -= maxPacketSize[numHops];
        }
      } else {
        if (remainingSize <= maxPacketSize[numHops]) {
          xbeeSerial.getFrame(buff, '4', remainingSize, '\n', enable_acks);
          for (int i = 0; i < remainingSize + 18; i++) {
            xbee_Serial.write(buff[i] & 0xFF);
          }
          packetId = 1;
          packet_sent = true;
        } else {
          xbeeSerial.getFrame(buff, '4', maxPacketSize[numHops], '-', enable_acks);
          for (int i = 0; i < maxPacketSize[numHops] + 18; i++) {
            xbee_Serial.write(buff[i] & 0xFF);
          }
          packetId = 4;
          remainingSize -= maxPacketSize[numHops];
        }
      }
    }
  }
}
