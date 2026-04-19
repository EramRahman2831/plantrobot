#include <Arduino_RouterBridge.h>
#include <Servo.h>
#include <Arduino_Modulino.h>

#define DIR1 3
#define DIR2 4
#define DIR3 6
#define DIR4 7
#define EN1 2
#define EN2 5
#define SERVO1 9
#define SERVO2 12

Servo servo;
Servo servo2;
ModulinoThermo thermo;

void setup() {
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(DIR3, OUTPUT);
  pinMode(DIR4, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);

  servo.attach(SERVO1);
  servo.write(90);   // default position

  servo2.attach(SERVO2);
  servo2.write(0);   // closed by default

  Modulino.begin();
  thermo.begin();
  Monitor.begin();
  Monitor.println("starting");
}

//
// -------- movement --------
//
void forward(int t) {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, HIGH);
  delay(t);
  stopMotors();
}

void backward(int t) {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, LOW);
  delay(t);
  stopMotors();
}

void stopMotors() {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);
  delay(300);
}

//
// -------- water dispense --------
//
void waterDispense() {
  float humidity = thermo.getHumidity();
  Monitor.print("Humidity: ");
  Monitor.println(humidity);

  servo2.write(180);   // open
  delay(1000);
  servo2.write(0);     // close
}

//
// -------- seed drop --------
//
void dropSeed() {
  Monitor.println("dropping seed");
  servo.write(0);
  delay(1000);
  servo.write(90);
  delay(1000);
}

//
// -------- simple movement path --------
//
void path(int i) {
  int j = 0;

  while (j < i) {

    // forward
    forward(3000);
    forward(1000);
    forward(3000);
    forward(1000);

    // backward (return)
    backward(1000);
    backward(3000);
    backward(1000);
    backward(3000);

    j++;
  }
}

//
// -------- seed path --------
//
void path1(int i) {
  int j = 0;

  while (j < i) {

    // forward + seed drops
    forward(1000); dropSeed();
    forward(1000); dropSeed();
    forward(1000); dropSeed();

    forward(1000); dropSeed();
    forward(1000); dropSeed();
    forward(1000); dropSeed();

    forward(1000); dropSeed();
    forward(1000); dropSeed();
    forward(1000); dropSeed();

    // backward return
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);

    j++;
  }
}

//
// -------- watering path --------
//
void path2(int i) {
  int j = 0;

  while (j < i) {

    // forward + water
    forward(1000); waterDispense();
    forward(1000); waterDispense();
    forward(1000); waterDispense();

    forward(1000); waterDispense();
    forward(1000); waterDispense();
    forward(1000); waterDispense();

    forward(1000); waterDispense();
    forward(1000); waterDispense();
    forward(1000); waterDispense();

    // backward return
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);
    backward(1000);

    j++;
  }
}

//
// -------- loop control --------
//
bool firstRun = true;
unsigned long lastWaterTime = 0;

void loop() {
  float humidity = thermo.getHumidity();
  Monitor.print("Humidity: ");
  Monitor.println(humidity);

  unsigned long currentTime = millis();
  unsigned long interval;

  if (humidity < 40) {
    interval = 6UL * 60 * 60 * 1000;
  } else if (humidity < 70) {
    interval = 12UL * 60 * 60 * 1000;
  } else {
    interval = 24UL * 60 * 60 * 1000;
  }

  if (firstRun) {
    path1(1);   // seed once
    path2(2);   // water twice
    lastWaterTime = currentTime;
    firstRun = false;
  }

  if (currentTime - lastWaterTime >= interval) {
    Monitor.println("Watering...");
    path2(2);
    lastWaterTime = currentTime;
  } else {
    path(2);
  }
}