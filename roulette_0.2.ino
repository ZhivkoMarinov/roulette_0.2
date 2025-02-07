#include "roulette_0.2.h"

wheel wheelControl = {
    .isWheelRotating = false,
    .isBallThere = false,
    .isWheelUp = false,
    .error = false,
};

int attemptsToRetrieve = 0;
int roundCounter = 0; //Remove after tests!

void setup() {
  delay(powerOnDelay);
  pinMode(winSensorPin, INPUT);
  pinMode(wheelSensorPin, INPUT_PULLUP);
  pinMode(controlWheelSensorPin, INPUT_PULLUP);
  pinMode(ballSensorPin, INPUT_PULLUP);
  pinMode(retrieveSensorPin, INPUT_PULLUP);
  pinMode(motorACSignal, OUTPUT);
  pinMode(motorACON, OUTPUT);
  pinMode(wheelLifter, OUTPUT);
  pinMode(ballFan, OUTPUT);
  Serial.begin(serialBitRate);
  emergencyStop();  
}

void loop() {
  playRound();
}

void playRound() {
  
  gameRound newGameRound = {
    .wheelSectorCounter = 0,
    .isSectorCounted = false,
    .statusWinSensor = false,
    .statusWheelSensor = false,
    .statusControlSensor = false,
    .isNumberRead = false
  };
    
  sendEvent(SNRS);
  retrieveBall(&wheelControl);
  spinWheel(&wheelControl);
  sendEvent(SBTS);
  delay(bettingTime);
  sendEvent(SBTE);
  fireBall(&wheelControl, &newGameRound);
  sendEvent(SRS);
  readNumber(&newGameRound);
  sendEvent(SRE);
  stopWheel(&wheelControl);
  sendEvent(SRF);
  Serial.print("Round ID: ");
  Serial.println(roundCounter);
}

void displayFreeRam() {
  Serial.print("RAM left: ");
  Serial.println(freeRam());  
}

int freeRam(){
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);  
}

volatile void sendError(String error) {
  if (error = EBNF) {
    retrieveBall(&wheelControl);
    playRound();
  }

  while(Serial.available() <= 0) {
    if(!wheelControl.error){
      Serial.println(error);
      wheelControl.error = true;
      emergencyStop();
    }
    delay(1000);
  }
  Serial.print("Current round count: ");
  Serial.println(roundCounter);
  playRound();
}

void emergencyStop() {
  digitalWrite(motorACSignal, HIGH);
  delay(stopWheelDuration);
  digitalWrite(motorACON, LOW);
}

void sendEvent(String event) {
  if (event && event.length() > 2){
    Serial.println(event);
  }
  else {
    sendError(EEM);
  }
}

void spinWheel(wheel *wheelControl) {
  if (wheelControl->isWheelRotating == false) {
    digitalWrite(motorACSignal, HIGH);
    delay(1500);
    digitalWrite(motorACON, HIGH);
    wheelControl->isWheelRotating = true;  
  }
}

void stopWheel(wheel *wheelControl) {
  if(wheelControl->isWheelRotating == true){
    digitalWrite(motorACON, LOW);
    delay(stopWheelDuration);
    digitalWrite(motorACSignal, LOW);    
    wheelControl->isWheelRotating = false;  
  }
}

void wheelUp(wheel *wheelControl) {
  if(wheelControl->isWheelUp == false) {
    while(digitalRead(retrieveSensorPin)) {
      digitalWrite(wheelLifter, HIGH);  
    }
    digitalWrite(wheelLifter, LOW);    
    wheelControl->isWheelUp = true;
  }
}

void wheelDownByTime(wheel *wheelControl) {
  if(wheelControl->isWheelUp) {
    digitalWrite(wheelLifter, HIGH);
    delay(100);
    digitalWrite(wheelLifter, LOW);
    wheelControl->isWheelUp = false;
  }
}

void wheelDownBySensor(wheel *wheelControl) {
  if(wheelControl->isWheelUp) {
    while(!digitalRead(retrieveSensorPin)) {
      digitalWrite(wheelLifter, HIGH);
    }
    digitalWrite(wheelLifter, LOW);
    wheelControl->isWheelUp = false;
  }
}

void retrieveBall(wheel *wheelControl) {
  wheelUp(wheelControl);
  delay(keepWheelUpDuration);
  wheelDownByTime(wheelControl);
}

void testFire() {
  digitalWrite(ballFan, HIGH);
  delay(2500);
  digitalWrite(ballFan, LOW);
}

void fireBall(wheel *wheelControl, gameRound *gameRound) {
  if(!digitalRead(ballSensorPin)) {
    const unsigned long current_time = millis();
    while (current_time + fireBallDuration > millis()) {
      digitalWrite(ballFan, HIGH);
    }
    digitalWrite(ballFan, LOW);
  }
  else{
    sendError(EBNF);
  }
}

void updateSensorStatus(int sensor, bool *sensorStatus) {
  
  if (!digitalRead(sensor)) {
    if (*sensorStatus == false) {
      *sensorStatus = true;
    }
  }
  else {
    if (*sensorStatus == true) {
      *sensorStatus = false;  
    }
  }
}

void winSectorCheck(gameRound *gameRound){
  
  if (gameRound->winningSectorCorrectCount == 0) {
    gameRound->winningSector = gameRound->wheelSectorCounter;
    gameRound->winningSectorCorrectCount++;
  }
  else{
    if(gameRound->winningSector == gameRound->wheelSectorCounter){
      gameRound->winningSectorCorrectCount++;
    }
    else {
      gameRound->winningSectorCorrectCount = 0;
    }
  }
}

void updateSectorCounter(gameRound *gameRound){
  if (gameRound->statusWheelSensor) {
    if (!gameRound->isSectorCounted) { 
      gameRound->wheelSectorCounter++;
      gameRound->isSectorCounted = true; 
    }
  }
  else{
      gameRound->isSectorCounted = false;
  }
  if (gameRound->statusControlSensor){
    gameRound->wheelSectorCounter = 0;
  }
}

void readNumber(gameRound *gameRound) {
  gameRound->winningSectorCorrectCount = 0;
  unsigned long current_time = millis();
  while (gameRound->winningSectorCorrectCount < 3){
    updateSensorStatus(wheelSensorPin, &gameRound->statusWheelSensor);
    updateSensorStatus(winSensorPin, &gameRound->statusWinSensor);
    updateSensorStatus(controlWheelSensorPin, &gameRound->statusControlSensor);
    updateSectorCounter(gameRound);
    
    if (digitalRead(winSensorPin)){
      if (gameRound->isNumberRead == false){
        gameRound->isNumberRead = true;
        winSectorCheck(gameRound);
      }
    }
    else{
      gameRound->isNumberRead = false;  
    }
    if (current_time + maxReadNumberTime < millis()) {
      sendError(ECRN);
    }
  }
  Serial.println(numberMap[gameRound->winningSector]);
  gameRound->winningSectorCorrectCount = 0;
  roundCounter++; //REMOVE THIS AFTER TESTS
}

void readNumberTest() {
  delay(10000);
  Serial.println("42");
  roundCounter++;
}
