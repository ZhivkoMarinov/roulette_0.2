#ifndef ROULETTE_0_2_H
#define ROULETTE_0_2_H

#include <stdbool.h>

// Error(E) codes
#define EBNF "BALL_NOT_FOUND"
#define ECRN "CAN'T_READ_NUMBER"
#define EMNS "MOTOR_NOT_SPINNING"
#define EEM "EVENT MISSING"

// State(S) codes 
#define SNRS "NEW_ROUND_START"
#define SBTS "BETTING_TIME_START"
#define SBTE "BETTING_TIME_END"
#define SBF "BALL_FIRE"
#define SRS "RESULT"
#define SRF "ROUND_FINISHED"

const unsigned long maxRetrieveTime = 10000;
const unsigned long maxReadNumberTime = 60000;
const int lookingForBallDuration = 5000;
const int fireBallDuration = 3000;
const int keepWheelUpDuration = 2500;
const int bettingTime = 18000;
const int powerOnDelay = 5000;
const int stopWheelDuration = 9000;
const int delayBetweenACmotorRelays = 1500;
const int delayWheelDown = 100; //If wheelDownByTime strategy is selected in the retrieveBall function, adjust this value by observation if needed!

const int correctCountThreshold = 3;

const int winSensorPin = 2;
const int wheelSensorPin = 3;
const int controlWheelSensorPin = 4;
const int ballSensorPin = 5;
const int retrieveSensorPin = 6;
const int motorACSignal = 8;
const int wheelLifter = 9;
const int ballFan = 10;
const int motorACON = 11;
const long int serialBitRate = 9600;

typedef struct gameRound {
  int wheelSectorCounter;
  bool isSectorCounted;
  bool statusWinSensor;
  bool statusWheelSensor;
  bool statusControlSensor;
  bool isNumberRead;
  int winningSector;
  int winningSectorCorrectCount;
} gameRound;

typedef struct wheelControl {
  bool isWheelRotating;
  bool isBallThere;
  bool isWheelUp;
  bool error;
} wheel;

const int numbersArray[] = {
    5, 10, 23, 8, 30, 11, 36, 13, 27, 6, 34, 17, 25, 2, 21, 4, 19, 15, 32, 0, 26, 3, 35, 12, 28, 7, 29, 18, 22, 9, 31, 14, 20, 1, 33, 16, 24
};
const int numbersArrayLength = 37; 

#endif
