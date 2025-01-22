#include <stdbool.h>
#include "roulette_0.2.h"

#define BDR "BALL_DIDNT_RETRIEVE"
#define CRN "CANT_READ_NUMBER"
#define MNS "MOTOR_NOT_SPINNING"

#define NRS "NEW_ROUND_START"
#define BTS "BETTING_TIME_START"
#define BTE "BETTING_TIME_END"
#define BF "BALL_FIRE"
#define RS "RESULT_START"
#define RE "RESULT_END"
#define RF "ROUND_FINISHED"

const int winSensorPin = 2;
const int wheelSensorPin = 3;
const int controlWheelSensorPin = 4;
const int ballSensorPin = 5;
const int retrieveSensorPin = 6;
const int motorACSignal = 8;
const int motorACON = 11;
const int serialBitRate = 9600;
const int bettingTime = 18;

typedef struct gameRound {
  unsigned long roundTime;
  int wheelSectorCounter;
  bool isSectorCounted;
  bool statusWinSensor;
  bool statusWheelSensor;
  bool statusControlSensor;
  bool isNumberRead;
  String statusCode;
  String state;
  int winningSector;
  int winningSectorCorrectCount;
} gameRound;

typedef struct wheelControl {
  bool isWheelRotating;
  bool isBallThere;
  bool isWheelUp;
} wheel;

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
  pinMode(motorACSignal, OUTPUT);
  pinMode(motorACON, OUTPUT);
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
  
  //spinWheel(&wheelControl);
  digitalWrite(motorACSignal, HIGH);
  delay(1000);
  digitalWrite(motorACON, HIGH);
  readNumber(&newGameRound);
  digitalWrite(motorACON, LOW);
  delay(10000);
  digitalWrite(motorACSignal, LOW);
  delay(2000);
  //stopWheel(&wheelControl);
  
}

int announceResult() {
return 0;
}

int spinWheel(wheel *wheelControl) {
  if (wheelControl->isWheelRotating == false){
    digitalWrite(motorACON, HIGH);
    digitalWrite(motorACSignal, HIGH);
    wheelControl->isWheelRotating = true;  
  }
  return 0;
}

int stopWheel(wheel *wheelControl) {
  if(wheelControl->isWheelRotating == true){
    digitalWrite(motorACSignal, LOW);
    delay(5000);
    digitalWrite(motorACON, LOW);    
    wheelControl->isWheelRotating = false;  
  }
  return 0;
}

int retrieveBall() {
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
  //while(true){
    updateSensorStatus(wheelSensorPin, &gameRound->statusWheelSensor);
    updateSensorStatus(winSensorPin, &gameRound->statusWinSensor);
    updateSensorStatus(controlWheelSensorPin, &gameRound->statusControlSensor);
    updateSectorCounter(gameRound);
    
    if (digitalRead(winSensorPin)){
      if (gameRound->isNumberRead == false){
        //Serial.println(gameRound->wheelSectorCounter);
        gameRound->isNumberRead = true;
        winSectorCheck(gameRound);
      }
    }
    else{
      gameRound->isNumberRead = false;  
    }
  }
  Serial.println(gameRound->winningSector);
  gameRound->winningSectorCorrectCount = 0;
}
