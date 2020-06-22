#define TEST_PERIOD_SHORT 1000
#define TEST_PERIOD_LONG 30000

char received = 0;
long bytesRXed_short = 0;
long lastMsg_short = 0;
double throughput_short = 0;
long bytesRXed_long = 0;
long lastMsg_long = 0;
double throughput_long = 0;
long now = 0;

short packetNumber = 1;
int bytesInThePacket = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    received = Serial.read();
    if (received == '\n') {
      bytesRXed_short += bytesInThePacket + 1;
      bytesRXed_long += bytesInThePacket + 1;
      bytesInThePacket = 0;
      packetNumber = 1;
    } else if (received == '-') {
      packetNumber++;
      bytesInThePacket++;
    } else if (received == '0') {
      bytesInThePacket++;
    } else if (received == '1') {
      if (packetNumber != 1) {
        bytesInThePacket = 1;
        packetNumber = 1;
      } else {
        bytesInThePacket++;
      }
    } else if (received == '2') {
      if (packetNumber != 2) {
        bytesInThePacket = 0;
      } else {
        bytesInThePacket++;
      }
    } else if (received == '3') {
      if (packetNumber != 3) {
        bytesInThePacket = 0;
      } else {
        bytesInThePacket++;
      }
    } else if (received == '*') {
      if (packetNumber < 4) {
        bytesInThePacket = 0;
      } else {
        bytesInThePacket++;
      }
    }

//    Serial.print(received, HEX);
//    Serial.print(" ");
//    if (received == '\n' || received == '0' || received == '1' || received == '2' || received == '3' || received == '*' || received == '-') {
//      Serial.print(received);
//    } else {
//      Serial.print(received, HEX);
//      Serial.print(" ");
//    }
  }
  now = millis();
  if (now - lastMsg_short > TEST_PERIOD_SHORT) {
    Serial.print("# of bytes RXed (short) = ");
    Serial.println(bytesRXed_short);
    Serial.print("RX Throughput (short) = ");
    throughput_short = (double)bytesRXed_short * 8.0 / (double)TEST_PERIOD_SHORT;
    Serial.print(throughput_short);
    Serial.println(" kbps");
    lastMsg_short = now;
    bytesRXed_short = 0;
  }
  if (now - lastMsg_long > TEST_PERIOD_LONG) {
    Serial.print("           # of bytes RXed (long) = ");
    Serial.println(bytesRXed_long);
    Serial.print("           RX Throughput (long) = ");
    throughput_long = (double)bytesRXed_long * 8.0 / (double)TEST_PERIOD_LONG;
    Serial.print(throughput_long);
    Serial.println(" kbps");
    lastMsg_long = now;
    bytesRXed_long = 0;
  }
}
