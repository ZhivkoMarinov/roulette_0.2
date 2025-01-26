#include "roulette_0.2.h"

wheel wheelControl = {
    .isWheelRotating = false,
    .isBallThere = false,
    .isWheelUp = false  
};

void setup() {
  // put your setup code here, to run once:
  pinMode(winSensorPin, INPUT);
  pinMode(wheelSensorPin, INPUT_PULLUP);
  pinMode(controlWheelSensorPin, INPUT_PULLUP);
  pinMode(retrieveSensorPin, INPUT_PULLUP);
  pinMode(motorACSignal, OUTPUT);
  pinMode(motorACON, OUTPUT);
  pinMode(wheelLifter, OUTPUT);
  Serial.begin(serialBitRate);
  
}

void loop() {
  // put your main code here, to run repeatedly:
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

  //wheelUp(&wheelControl);
  retrieveBall(&wheelControl);
  spinWheel(&wheelControl);
  readNumber(&newGameRound);
  stopWheel(&wheelControl);
  //delay(5000); //-->REMOVE THIS AFTER TESTS!!!!!!!!!!!
  //wheelDown(&wheelControl);
  //delay(5000);
  
}

int sendError() {
return 0;
}

int spinWheel(wheel *wheelControl) {
  if (wheelControl->isWheelRotating == false){
    digitalWrite(motorACSignal, HIGH);
    delay(1000);
    digitalWrite(motorACON, HIGH);
    wheelControl->isWheelRotating = true;  
  }
  return 0;
}

int stopWheel(wheel *wheelControl) {
  if(wheelControl->isWheelRotating == true){
    digitalWrite(motorACON, LOW);
    delay(9000);
    digitalWrite(motorACSignal, LOW);    
    wheelControl->isWheelRotating = false;  
  }
  return 0;
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

int retrieveBall(wheel *wheelControl) {
  wheelUp(wheelControl);
  delay(3000);
  wheelDown(wheelControl);
  return 0;
}

int updateSensorStatus(int sensor, bool *sensorStatus) {
  
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
  return 0;
}

int winSectorCheck(gameRound *gameRound){
  
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
  return 0;
}

int updateSectorCounter(gameRound *gameRound){
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


int readNumber(gameRound *gameRound) {
  gameRound->winningSectorCorrectCount = 0;

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
  }
  Serial.println(numberMap[gameRound->winningSector]);
  gameRound->winningSectorCorrectCount = 0;
}
