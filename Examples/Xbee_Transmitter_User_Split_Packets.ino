#include <xbee_Arduino.h>

xbee xbeeSerial;
String response;

// 64-bit destination address for unicast trasmittion
char destination_address[8] =  {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB5, 0x8E, 0xAC};
// 64-bit destination address for broadcast trasmittion
char destination_address_broadcast[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
// 16-bit destination address for unicast trasmittion
char destination_address16[2] = {0x6B, 0x19};
// 16-bit destination address for broadcast trasmittion
char destination_address_broadcast_16[2] = {0xFF, 0xFE};
// Number of relay nodes
int numHops = 1;
// Relay nodes' 16-bit addresses
char route[6] = {0xB7, 0x4C, 0xE3, 0xE4, 0xAF, 0x9B};

// Maximum application layer packet size
short maxPacketSize[4] = {84, 80, 78, 76};
// delay between adjacent packets in ms
long delay_between_packets[4] = {0, 13, 20, 28};
int packetId = 1;
// Application layer payload size
int payloadSize = 152;
int remainingSize = payloadSize;

// Whether ACKs are enabled
bool enable_acks = false;

void setup() {
  xbeeSerial.begin(115200);
  Serial.begin(115200);
  Serial.println("online");
  xbeeSerial.create_route(destination_address, destination_address16, numHops, route);
  delay(1000);
}

void loop() {
  delay(delay_between_packets[numHops]);
  if (payloadSize <= maxPacketSize[numHops]) {
    xbeeSerial.sendata("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000", payloadSize, '\n', enable_acks);
  } else {
    if (packetId == 1) {
      xbeeSerial.sendata("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111", maxPacketSize[numHops], '-', enable_acks);
      remainingSize -= maxPacketSize[numHops];
      packetId = 2;
    } else if (packetId == 2) {
      if (remainingSize <= maxPacketSize[numHops]) {
        xbeeSerial.sendata("22222222222222222222222222222222222222222222222222222222222222222222222222222222222222", remainingSize, '\n', enable_acks);
        packetId = 1;
        remainingSize = payloadSize;
      } else {
        xbeeSerial.sendata("22222222222222222222222222222222222222222222222222222222222222222222222222222222222222", maxPacketSize[numHops], '-', enable_acks);
        packetId = 3;
        remainingSize -= maxPacketSize[numHops];
      }
    } else if (packetId == 3) {
      if (remainingSize <= maxPacketSize[numHops]) {
        xbeeSerial.sendata("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333", remainingSize, '\n', enable_acks);
        packetId = 1;
        remainingSize = payloadSize;
      } else {
        xbeeSerial.sendata("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333", maxPacketSize[numHops], '-', enable_acks);
        packetId = 4;
        remainingSize -= maxPacketSize[numHops];
      }
    } else {
      if (remainingSize <= maxPacketSize[numHops]) {
        xbeeSerial.sendata("44444444444444444444444444444444444444444444444444444444444444444444444444444444444444", remainingSize, '\n', enable_acks);
        packetId = 1;
        remainingSize = payloadSize;
      } else {
        xbeeSerial.sendata("44444444444444444444444444444444444444444444444444444444444444444444444444444444444444", maxPacketSize[numHops], '-', enable_acks);
        packetId = 4;
        remainingSize -= maxPacketSize[numHops];
      }
    }
  }
}
