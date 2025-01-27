#include "roulette_0.2.h"

wheel wheelControl = {
    .isWheelRotating = false,
    .isBallThere = false,
    .isWheelUp = false,
    .error = false
};

void setup() {
  delay(powerOnDelay);
  pinMode(winSensorPin, INPUT);
  pinMode(wheelSensorPin, INPUT_PULLUP);
  pinMode(controlWheelSensorPin, INPUT_PULLUP);
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
      .roundTime = millis(),
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
}

volatile void sendError(String error) {
  while(true) {
    if(!wheelControl.error){
      Serial.println(error);
      wheelControl.error = true;
      emergencyStop();
    }
    delay(1000);
  }
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
    delay(1000);
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

void wheelDown(wheel *wheelControl) {
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
  wheelDown(wheelControl);
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
}
