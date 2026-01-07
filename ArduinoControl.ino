//Arduino code for taking python inputs to control outputs

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Servo.h>


// BTS7960 motor driver 
#define RPWM 5
#define LPWM 6
#define REN  7
#define LEN  8

// Servos
#define FL_SERVO 9
#define FR_SERVO 10
#define NECK_SERVO 11



Servo flServo, frServo, neckServo;

byte mac[] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED};
IPAddress ip(192,168,1,50);
EthernetUDP Udp;

char buffer[64];

int speedCmd = 0;
int flAngle = 90;
int frAngle = 90;
int neckAngle = 90;

unsigned long lastPacket = 0;

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(5005);

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(REN, OUTPUT);
  pinMode(LEN, OUTPUT);

  digitalWrite(REN, HIGH);
  digitalWrite(LEN, LOW);

  flServo.attach(FL_SERVO);
  frServo.attach(FR_SERVO);
  neckServo.attach(NECK_SERVO);

  flServo.write(90);
  frServo.write(90);
  neckServo.write(90);
}
void loop() {
  int size = Udp.parsePacket();
  if (size) {
    int len = Udp.read(buffer, 63);
    buffer[len] = 0;

    sscanf(buffer,
      "SPD:%d,FL:%d,FR:%d,NECK:%d",
      &speedCmd, &flAngle, &frAngle, &neckAngle
    );

    lastPacket = millis();
  }

  // Failsafe
  if (millis() - lastPacket > 500) {
    speedCmd = 0;
  }

  analogWrite(RPWM, speedCmd);
  analogWrite(LPWM, 0);

  flServo.write(constrain(flAngle, 0, 180));
  frServo.write(constrain(frAngle, 0, 180));
  neckServo.write(constrain(neckAngle, 0, 180));
}

