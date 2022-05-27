#pragma once

#include <Arduino.h>
#include "BasicStepperDriver.h"
#include "MultiDriver.h"
#include "SyncDriver.h"
#include "GCodeParser.h"
#include <EEPROM.h>

#define MOTOR_STEPS_X_ADDR 0
#define MOTOR_STEPS_Y_ADDR 4

#define RPM_X_ADDR 8
#define RPM_Y_ADDR 12

#define MICROSTEPS_X_ADDR 16
#define MICROSTEPS_Y_ADDR 20

#define STEPS_PER_MM_X_ADDR 24
#define STEPS_PER_MM_Y_ADDR 28

#define OFFSET_X_ADDR 32
#define OFFSET_Y_ADDR 36

#define MAX_TRAVEL_X_ADDR 40
#define MAX_TRAVEL_Y_ADDR 44

#define MOTOR_STEPS_X 200
#define MOTOR_STEPS_Y 200

#define DIR_X 2
#define STEP_X 3
#define DIR_Y 4
#define STEP_Y 5

#define ENABLE 10

#define LIMIT_X 6
#define LIMIT_Y 7

#define AXIS_X  0
#define AXIS_Y  1

#define RAPID 0
#define LINEAR 1

class Controller
{
public:
  Controller();
  ~Controller();

public:
  void init();
  void available();
  void processCommand();
  bool commandReceived();
  
private:
  bool intepretCommand();
  bool intepretConfigCommand();
  void moveAbsolute(float X_millis, float Y_millis, int move);
  void moveRelative(float X_millis, float Y_millis, int move);
  void moveRapid(float X_millis, float Y_millis);
  void moveLinear(float X_millis, float Y_millis);
  void homingCycle();
  
  void checkLimits();
  void checkMaxTravel();
  void loadConfigs();
  
  void setRpm(int rpm, int axis);
  void setMicroStep(int microstep, int axis);
  void setStepsPerMM(float steps_per_mm, int axis);
  void setOffset(float offset, int axis);
  void setMaxTravel(float travel, int axis);

  void writeInfo(char *buf, float x, float y, int com);

  void delayMotor(float delayMillis);

  void commandError();
  void emergencyStop();

//Variables
public:
  enum MoveMode {ABSOLUTE, RELATIVE};

private:
  GCodeParser GCode = GCodeParser();
  BasicStepperDriver stepper_x =  BasicStepperDriver(MOTOR_STEPS_X, DIR_X, STEP_X, ENABLE);
  BasicStepperDriver stepper_y =  BasicStepperDriver(MOTOR_STEPS_Y, DIR_Y, STEP_Y, ENABLE);
  SyncDriver syncstepper =  SyncDriver(stepper_x, stepper_y);
  MultiDriver multistepper = MultiDriver(stepper_x, stepper_y);
  
  bool startCommand = false;
  bool homing = false;
  bool limit_reached_x = false;
  bool limit_reached_y = false;

  bool delayCheck = false;

  unsigned long timeBefore;

  int x_steps = 0;
  int y_steps = 0;

  int x_steps_done = 0;
  int y_steps_done = 0;

  int x_now = 0;
  int y_now = 0;

  float delayTime;

  MoveMode mode = ABSOLUTE;

// Configurations
  float RPM_X = 0;
  float RPM_Y = 0;
  
  float MICROSTEPS_X = 0;
  float MICROSTEPS_Y = 0;
  
  float STEPS_PER_MM_X = 0;
  float STEPS_PER_MM_Y = 0;
  
  float OFFSET_X = 0;
  float OFFSET_Y = 0;

  float MAX_TRAVEL_X = 0;
  float MAX_TRAVEL_Y = 0;
  
};
