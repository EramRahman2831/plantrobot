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
  pinMode(SERVO1, OUTPUT);
  pinMode(SERVO2, OUTPUT);
  

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  servo.attach(SERVO1);
  servo.write(90);
  servo2.attach(SERVO2);
  Modulino.begin();
  thermo.begin();
  Monitor.begin();
  Monitor.println("starting");
}


// ------- water dispense -----
void waterDispense() {
    float humidity = thermo.getHumidity();
    Monitor.print("Humidity: ");
    Monitor.println(humidity);
    servo2.write(180);   // open
    delay(1000);         // stay open 1 second
    servo2.write(0);     // close
  
}

// -------- movements --------
void forward(int t) {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, HIGH);
  delay(t);
  stopMotors();
}

void leftTurn(int t) {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, HIGH);
  delay(t);
  stopMotors();
}

void rightTurn(int t) {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
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

// -------- 180 degree turn --------
void turnAround() {
  rightTurn(1200);  // 
}

// -------- path --------
void path(int i) {
  int j = 0;
  
  while (j < i) {
    
    forward(3000);
    rightTurn(500);
  
    forward(1000);
    rightTurn(500);
  
    forward(3000);
    leftTurn(500);
  
    forward(1000);
    leftTurn(500);

    j++;
    
  }
}

// -------- path seed --------
void path1(int i) {

int j = 0;
  
  while (j < i) {
     
    forward(1000);
    dropSeed();
    forward(1000);
    dropSeed();
    forward(1000);
    dropSeed();
    rightTurn(500);  
      
    forward(1000);
    rightTurn(500);
  
    forward(1000);
    dropSeed();
    forward(1000);
    dropSeed();
    forward(1000);
    dropSeed();
    leftTurn(500);
  
    forward(1000);
    leftTurn(500);
    
    forward(1000);
    dropSeed();
    forward(1000);
    dropSeed();
    forward(1000);
    dropSeed();
    
    j++;
  }
  
}

// --------- water path --------
void path2(int i) {

int j = 0;
  
  while (j < i) {
     
    forward(1000);
    waterDispense();
    forward(1000);
    waterDispense();
    forward(1000);
    waterDispense();
    rightTurn(500);  
      
    forward(1000);
    rightTurn(500);
  
    forward(1000);
    waterDispense();
    forward(1000);
    waterDispense();
    forward(1000);
    waterDispense();
    leftTurn(500);
  
    forward(1000);
    leftTurn(500);
    
    forward(1000);
    waterDispense();
    forward(1000);
    waterDispense();
    forward(1000);
    waterDispense();
    
    j++;
  }
  
}


void dropSeed() {
  Monitor.println("dropping seed");
  servo.write(0);
  delay(1000);
  servo.write(90);
  delay(1000);
}

// -------- llops forever --------
bool firstRun = true;  // 
unsigned long lastWaterTime = 0;

void loop() {
  float humidity = thermo.getHumidity();
  Monitor.print("Humidity: ");
  Monitor.println(humidity);
  unsigned long currentTime = millis();

  unsigned long interval;

  if (humidity < 40) {
    interval = 6UL * 60 * 60 * 1000;   // 6 hours
  } else if (humidity < 70) {
    interval = 12UL * 60 * 60 * 1000;  // 12 hours
  } else {
    interval = 24UL * 60 * 60 * 1000;  // 24 hours (humid)
  }

  if (firstRun) {
    path1(1);
    turnAround();
    path2(2);
    turnAround();
    lastWaterTime = currentTime;
    firstRun = false;
  }

  // Check if enough time passed
  if (currentTime - lastWaterTime >= interval) {
    Monitor.println("Watering...");
    path2(2);
    turnAround();
    lastWaterTime = currentTime;
  } else {
    path(2);
    turnAround();
  }
}