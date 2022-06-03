#include "Controller.h"

Controller::Controller()
{
//  stepper_x = new BasicStepperDriver(MOTOR_STEPS, DIR_X, STEP_X, ENABLE);
//  stepper_y = new BasicStepperDriver(MOTOR_STEPS, DIR_Y, STEP_Y, ENABLE);
}
Controller::~Controller(){}

void Controller::init()
{ 
  pinMode(LIMIT_X, INPUT_PULLUP);
  pinMode(LIMIT_Y, INPUT_PULLUP);

  loadConfigs();

  stepper_x.begin(RPM_X, MICROSTEPS_X);
  stepper_y.begin(RPM_Y, MICROSTEPS_Y);

  stepper_x.setEnableActiveState(LOW);
  stepper_y.setEnableActiveState(LOW);
}

void Controller::available()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (GCode.AddCharToLine(c))
    {
      GCode.ParseLine();
      // Code to process the line of G-Code here…

//      Serial.print("Command Line: ");
//      Serial.println(GCode.line);
      
      if (GCode.HasWord('G'))
      {
//        Serial.println("---Processing G code---");
        if (intepretCommand())
          startCommand = true;
      }

      else if (GCode.HasWord('M'))
      {
//        Serial.println("--Configuring--");
        intepretConfigCommand();
      }

      else
      {
        commandError();
      }
    }
  } 
}

void Controller::setRpm(int rpm, int axis)
{
  switch (axis)
  {
    case AXIS_X:
      RPM_X = rpm;
      EEPROM.put(RPM_X_ADDR, RPM_X);
      stepper_x.setRPM(RPM_X);
      break;

    case AXIS_Y:
      RPM_Y = rpm;
      EEPROM.put(RPM_Y_ADDR, RPM_Y);
      stepper_y.setRPM(RPM_Y);
      break;
  }
}

void Controller::setMicroStep(int microstep, int axis)
{
  switch (axis)
  {
    case AXIS_X:
      MICROSTEPS_X = microstep;
      EEPROM.put(MICROSTEPS_X_ADDR, MICROSTEPS_X);
      stepper_x.setMicrostep(MICROSTEPS_X);
      break;
  
    case AXIS_Y:
      MICROSTEPS_Y = microstep;
      EEPROM.put(MICROSTEPS_Y_ADDR, MICROSTEPS_Y);
      stepper_y.setMicrostep(MICROSTEPS_Y);  
      break;
  }
}

void Controller::setStepsPerMM(float steps_per_mm, int axis)
{
  switch (axis)
  {
    case AXIS_X:
      STEPS_PER_MM_X = steps_per_mm;
      EEPROM.put(STEPS_PER_MM_X_ADDR, STEPS_PER_MM_X);
      break;

    case AXIS_Y:
      STEPS_PER_MM_Y = steps_per_mm;
      EEPROM.put(STEPS_PER_MM_Y_ADDR, STEPS_PER_MM_Y);
      break;
  }
}

void Controller::setOffset(float offset, int axis)
{
  switch (axis)
  {
    case AXIS_X:
      OFFSET_X = offset;
      EEPROM.put(OFFSET_X_ADDR, OFFSET_X);
      break;

    case AXIS_Y:
      OFFSET_Y = offset;
      EEPROM.put(OFFSET_Y_ADDR, OFFSET_Y);
      break;
  }
}

void Controller::setMaxTravel(float travel, int axis)
{
  switch (axis)
  {
    case AXIS_X:
      MAX_TRAVEL_X = travel;
      EEPROM.put(MAX_TRAVEL_X_ADDR, MAX_TRAVEL_X);
      break;

    case AXIS_Y:
      MAX_TRAVEL_Y = travel;
      EEPROM.put(MAX_TRAVEL_Y_ADDR, MAX_TRAVEL_Y);
      break;
  }
}

void Controller::loadConfigs()
{
    EEPROM.get(RPM_X_ADDR, RPM_X);
    if (isnan(RPM_X))
      RPM_X = 120;

    EEPROM.get(RPM_Y_ADDR, RPM_Y);
    if (isnan(RPM_Y))
      RPM_Y = 120;
      
    EEPROM.get(MICROSTEPS_X_ADDR, MICROSTEPS_X);
    if (isnan(MICROSTEPS_X))
      MICROSTEPS_X = 1;

    EEPROM.get(MICROSTEPS_Y_ADDR, MICROSTEPS_Y);
    if (isnan(MICROSTEPS_Y))
      MICROSTEPS_Y = 1;

    EEPROM.get(STEPS_PER_MM_X_ADDR, STEPS_PER_MM_X);
    if (isnan(STEPS_PER_MM_X))
      STEPS_PER_MM_X = 25;

    EEPROM.get(STEPS_PER_MM_Y_ADDR, STEPS_PER_MM_Y);
    if (isnan(STEPS_PER_MM_Y))
      STEPS_PER_MM_Y = 25;

    EEPROM.get(OFFSET_X_ADDR, OFFSET_X);
    if (isnan(OFFSET_X))
      OFFSET_X = 1;

    EEPROM.get(OFFSET_Y_ADDR, OFFSET_Y);
    if (isnan(OFFSET_Y))
      OFFSET_Y = 1;

    EEPROM.get(MAX_TRAVEL_X_ADDR, MAX_TRAVEL_X);
    if (isnan(MAX_TRAVEL_X))
      MAX_TRAVEL_X = 300;

    EEPROM.get(MAX_TRAVEL_Y_ADDR, MAX_TRAVEL_Y);
    if (isnan(MAX_TRAVEL_Y))
      MAX_TRAVEL_Y = 200;
}

bool Controller::intepretCommand()
{
  float X = NAN;
  float Y = NAN;
  float P = 0.0;
  bool commandIntepreted = false;
  
  switch ((int)GCode.GetWordValue('G'))
  { 
    case 0:
      if (GCode.HasWord('X'))
        X = (float)GCode.GetWordValue('X');
      if (GCode.HasWord('Y'))
        Y = (float)GCode.GetWordValue('Y');
      moveRapid(X, Y);
      commandIntepreted = true;
      break;

    case 1:
      if (GCode.HasWord('X'))
        X = (float)GCode.GetWordValue('X');
      if (GCode.HasWord('Y'))
        Y = (float)GCode.GetWordValue('Y');
      moveLinear(X, Y);
      commandIntepreted = true;
      break;

    case 4:
      if (GCode.HasWord('P'))
        P = (float)GCode.GetWordValue('P');
      delayMotor(P);
      commandIntepreted = true;
      break;

    case 28:
      homingCycle();
      commandIntepreted = true;
      break;

    case 90:
      mode = ABSOLUTE;
      x_steps = 0;
      y_steps = 0;
      commandIntepreted = true;
      break;

    case 91:
      mode = RELATIVE;
      x_steps = 0;
      y_steps = 0;
      commandIntepreted = true;
      break;

    default:
      commandError();
      commandIntepreted = false;
      break;
  }
  return commandIntepreted;
}

bool Controller::intepretConfigCommand()
{
  float X = 0.0; 
  float Y = 0.0;
  char returnCommand[50];
  bool hasX = false;
  bool hasY = false;

  if (GCode.HasWord('X'))
  {
    X = (float)GCode.GetWordValue('X');
    hasX = true;
  }
  if (GCode.HasWord('Y'))
  {
    Y = (float)GCode.GetWordValue('Y');
    hasY = true;
  }
  
  switch ((int)GCode.GetWordValue('M'))
  { 
    case 100:
      if (hasX)
        setRpm(X, AXIS_X);
 
      if (hasY)
        setRpm(Y, AXIS_Y);
        
      writeInfo(returnCommand, RPM_X, RPM_Y, 100);
      Serial.write(returnCommand);
      break;

    case 101:
      if (hasX)
        setMicroStep(X, AXIS_X);
      
      if (hasY)
        setMicroStep(Y, AXIS_Y);
      
      writeInfo(returnCommand, MICROSTEPS_X, MICROSTEPS_Y, 101);
      Serial.write(returnCommand);
      break;
      
    case 102:
      if (hasX)
        setStepsPerMM(X, AXIS_X);
      
      if (hasY)
        setStepsPerMM(Y, AXIS_Y);
      
      writeInfo(returnCommand, STEPS_PER_MM_X, STEPS_PER_MM_Y, 102);
      Serial.write(returnCommand);
      break;

    case 103:
      if (hasX)
        setOffset(X, AXIS_X);
      
      if (hasY)
        setOffset(Y, AXIS_Y);
      
      writeInfo(returnCommand, OFFSET_X, OFFSET_Y, 103);
      Serial.write(returnCommand);
      break;

    case 114:
      writeInfo(returnCommand, x_now / (STEPS_PER_MM_X * MICROSTEPS_X), y_now / (STEPS_PER_MM_Y * MICROSTEPS_Y), 114);
      Serial.write(returnCommand);
      break;

    case 112:
      emergencyStop();
      break;

    case 208:
      if (hasX)
        setMaxTravel(X, AXIS_X);

      if (hasY)
        setMaxTravel(Y, AXIS_Y);

      writeInfo(returnCommand, MAX_TRAVEL_X, MAX_TRAVEL_Y, 208);
      Serial.write(returnCommand);
      break;
      
    default:
      commandError();
      break;
  }
  return true;
}

void Controller::writeInfo(char *buf, float x, float y, int com)
{
  char x_buf[10];
  char y_buf[10];
  
  dtostrf(x, 4, 2, x_buf);
  dtostrf(y, 4, 2, y_buf);
  sprintf(buf, "M%i X%s Y%s\n", com, x_buf, y_buf);
}

void Controller::moveAbsolute(float X_millis, float Y_millis, int move)
{
  if (isnan(X_millis))
    x_steps = stepper_x.getStepsRemaining();
  else
    x_steps = ((X_millis * STEPS_PER_MM_X * MICROSTEPS_X) - x_now);

  if (isnan(Y_millis))
    y_steps = stepper_y.getStepsRemaining();
  else
    y_steps = ((Y_millis * STEPS_PER_MM_Y * MICROSTEPS_Y) - y_now);

//  Serial.println(String(x_steps) + " " + String(y_steps));

  checkMaxTravel();

  switch (move)
  {
    case LINEAR:
      syncstepper.enable();
      syncstepper.startMove(-x_steps, -y_steps);
      break;
  
    case RAPID:
      multistepper.enable();
      multistepper.startMove(-x_steps, -y_steps);
      break;
  }
} 

void Controller::moveRelative(float X_millis, float Y_millis, int move)
{
  if (isnan(X_millis))
    x_steps = stepper_x.getStepsRemaining();
  else
    x_steps = X_millis * STEPS_PER_MM_X * MICROSTEPS_X;

  if (isnan(Y_millis))
    y_steps = stepper_y.getStepsRemaining();
  else
    y_steps = Y_millis * STEPS_PER_MM_Y * MICROSTEPS_Y;

  checkMaxTravel();

//  Serial.println(String(x_steps) + " " + String(y_steps));
  
  switch (move)
  {
    case LINEAR:
      syncstepper.enable();
      syncstepper.startMove(-x_steps, -y_steps);
      break;
  
    case RAPID:
      multistepper.enable();
      multistepper.startMove(-x_steps, -y_steps);
      break;
  }
} 

void Controller::moveRapid(float X_millis, float Y_millis)
{
  switch(mode)
  {
    case ABSOLUTE:
      moveAbsolute(X_millis, Y_millis, RAPID);
      break;

    case RELATIVE:
      moveRelative(X_millis, Y_millis, RAPID);
      break;
  }
}

void Controller::moveLinear(float X_millis, float Y_millis)
{
  switch(mode)
  {
    case ABSOLUTE:
      moveAbsolute(X_millis, Y_millis, LINEAR);
      break;

    case RELATIVE:
      moveRelative(X_millis, Y_millis, LINEAR);
      break;
  }
}

void Controller::homingCycle()
{
  homing = true;
  moveRelative(-MAX_TRAVEL_X, -MAX_TRAVEL_Y, RAPID);
}

void Controller::delayMotor(float delayMillis)
{
  delayCheck = true;
  timeBefore = millis();
  delayTime = delayMillis;

  x_steps = 0;
  y_steps = 0;
}

void Controller::processCommand() 
{
  //Check limitswitches
  if (digitalRead(LIMIT_X))
  {
  limit_reached_x = true;
  stepper_x.stop();
  }

  if (digitalRead(LIMIT_Y))
  {
  limit_reached_y = true;
  stepper_y.stop();
  }

  //Check if delay
  if (delayCheck)
  {
    if ((millis() - timeBefore) > delayTime)
      delayCheck = false;
    return;
  }
   
  unsigned wait_time_micros = multistepper.nextAction();
  if (wait_time_micros <= 0) {
    //Calculate steps done
    x_steps_done = (x_steps == 0) ? 0 : stepper_x.getStepsCompleted();
    x_steps_done = (x_steps >= 0) ? x_steps_done : -x_steps_done;
    x_now = x_now + x_steps_done;

    y_steps_done = (y_steps == 0) ? 0 : stepper_y.getStepsCompleted();
    y_steps_done = (y_steps >= 0) ? y_steps_done : -y_steps_done;
    y_now = y_now + y_steps_done;

    checkLimits(); //Do extra functions if limit has been triggered. This includes homing,

    delay(100); //Wait 100ms before disabling motor
    multistepper.disable(); 
    
    Serial.write("OK\n"); 
    
    startCommand = false;
  }
}

void Controller::checkLimits()
{
  float x_unstuck_steps = 0;
  float y_unstuck_steps = 0;
  
  if (limit_reached_x)
  { 
    x_unstuck_steps = (x_steps >= 0) ? -OFFSET_X: OFFSET_X;
    x_unstuck_steps = x_unstuck_steps * MICROSTEPS_X * STEPS_PER_MM_X;
    //x_now = x_now + (int)x_unstuck_steps;
  }

  if (limit_reached_y)
  {
    y_unstuck_steps = (y_steps >= 0) ? -OFFSET_Y: OFFSET_Y;
    y_unstuck_steps = y_unstuck_steps * MICROSTEPS_Y * STEPS_PER_MM_Y;
    //y_now = y_now + (int)y_unstuck_steps;
  }

  if (homing)
  {  
    x_now = 0;
    y_now = 0;
//    homing = false;
  }
    
  if ((limit_reached_x || limit_reached_y) && homing)
  {
    delay(1000);
    multistepper.move(-(int)x_unstuck_steps, -(int)y_unstuck_steps);
    limit_reached_x = false;
    limit_reached_y = false;
    homing  = false;
  }
}

void Controller::checkMaxTravel()
{
  if (!homing)
  {
    int Max_x_steps = MAX_TRAVEL_X * STEPS_PER_MM_X * MICROSTEPS_X;
    int Max_y_steps = MAX_TRAVEL_Y * STEPS_PER_MM_Y * MICROSTEPS_Y;
    
    if (x_now + x_steps >= Max_x_steps)
      x_steps = Max_x_steps - x_now;
    else if (x_now + x_steps < 0)
      x_steps = -x_now;
      
    if (y_now + y_steps >= Max_y_steps)
      y_steps = Max_y_steps - y_now;
    else if (y_now + y_steps < 0)
      y_steps = -y_now;
  }
}

void Controller::emergencyStop()
{
  stepper_x.stop();
  stepper_y.stop();
  if (!startCommand)
  {
    startCommand = true;
    x_steps = 0;
    y_steps = 0;
  }
}

bool Controller::commandReceived()
{
  return startCommand;
}

void Controller::commandError()
{
  Serial.write("GCode not recognized\n");
}
