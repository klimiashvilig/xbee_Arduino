#define TEST_PERIOD_SHORT 1000
#define TEST_PERIOD_LONG 30000

char received = 0;

short packetNumber = 1;
int bytesInThePacket = 0;

int counter = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    received = Serial.read();
    if (received == '\n') {
      packetNumber = 1;
      counter++;
      Serial.println(counter);
    } else if (received == '-') {
      packetNumber++;
    } else if (received == '1') {
      if (packetNumber != 1) {
        packetNumber = 1;
      }
    }

//    Serial.print(received, HEX);
//    Serial.print(" ");
    if (received == '\n' || received == '0' || received == '1' || received == '2' || received == '3' || received == '*' || received == '-') {
//      Serial.print(received);
//    } else {
//      Serial.print(received, HEX);
//      Serial.print(" ");
    }
  }
}
