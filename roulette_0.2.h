#ifndef ROULETTE_0_2_H
#define ROULETTE_0_2_H

#include <stdbool.h>

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
const int wheelLifter = 9;
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

const int numberMap[] = {
    5, 10, 23, 8, 30, 11, 36, 13, 27, 6, 34, 17, 25, 2, 21, 4, 19, 15, 32, 0, 26, 3, 35, 12, 28, 7, 29, 18, 22, 9, 31, 14, 20, 1, 33, 16, 24
}; 

#endif
