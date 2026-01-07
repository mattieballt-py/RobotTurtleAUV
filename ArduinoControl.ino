//Arduino code for taking python inputs to control outputs

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Servo.h>

// Servos
const int LEFT_SERVO_PIN = 6;
const int RIGHT_SERVO_PIN = 7;

// Motor driver
const int MOTOR_EN_PIN = 8;   // Enable pin on BTS7960

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 50);
unsigned int localPort = 5005;

EthernetUDP Udp;

Servo leftServo;
Servo rightServo;

char packetBuffer[64];

int motorState = 0;
int leftAngle = 90;
int rightAngle = 90;

unsigned long lastPacketTime = 0;
const unsigned long FAILSAFE_TIMEOUT = 500; // ms

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);

  pinMode(MOTOR_EN_PIN, OUTPUT);
  digitalWrite(MOTOR_EN_PIN, LOW);

  leftServo.write(90);
  rightServo.write(90);
}

void parsePacket(char* data) {
  sscanf(data, "M:%d,L:%d,R:%d", &motorState, &leftAngle, &rightAngle);

  leftAngle = constrain(leftAngle, 0, 180);
  rightAngle = constrain(rightAngle, 0, 180);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 63);
    if (len > 0) packetBuffer[len] = 0;

    parsePacket(packetBuffer);
    lastPacketTime = millis();
  }

  // Failsafe: stop motor if comms lost
  if (millis() - lastPacketTime > FAILSAFE_TIMEOUT) {
    digitalWrite(MOTOR_EN_PIN, LOW);
  } else {
    digitalWrite(MOTOR_EN_PIN, motorState ? HIGH : LOW);
  }

  leftServo.write(leftAngle);
  rightServo.write(rightAngle);
}

