#include <Evo.h>
#include <SoftwareSerial.h>

EVOX1 evo;
EvoHuskyLens hl(I2C4);
EvoBNO055 bno(I2C3);

EvoVL53L0X tof_left(I2C1);
EvoVL53L0X tof_right(I2C2);

EvoVL53L0X tof_front(I2C5);
EvoMotor driveMotor(M2, EV3MediumMotor, false);
EvoMotor steerMotor(M3, EV3MediumMotor, true);

HUSKYLENSResult result;

// IMU variables
float x, y, z;
float heading;
float target_heading = 0;
int TurnRound = 0;
int RoundMod = 0;
int directionmult = 0;
int distanceAbs = 0;
int distanceAbsNeg = 0;
const float Kp = 2.3;
const int MAX_STEER_ANGLE = 50;
const int driveSpeed = 3300;

bool turning = false;
bool isManeuvering = false;

void updateHeading() {
  bno.getEuler(&x, &y, &z);
  heading = x;
  if (heading < 0) heading += 360;
  if (heading >= 360) heading -= 360;
}

float calculateHeadingError(float current, float target) {
  float error = target - current;
  if (error > 180) error -= 360;
  if (error < -180) error += 360;
  return error;
}
void setup() {
  hl.begin();
  hl.setMode(ALGORITHM_COLOR_RECOGNITION);
  hl.writeString("Color Recognition", 0, 0);
  Serial0.begin(115200);
  evo.begin();
  bno.begin();
  steerMotor.begin();
  driveMotor.begin();
  tof_left.begin();
  tof_right.begin();
  tof_front.begin();

  steerMotor.resetAngle();
  driveMotor.resetAngle();

  evo.writeLineToDisplay("Gyro + Wall Tracking + Blocks", 0, true, false);
  evo.writeToDisplay(evo.getBattery(), 50, 16, false, false);
  evo.drawDisplay();

  delay(500);
  evo.waitForBump();
  delay(1000);
  hl.clearScreen();

  updateHeading();
  TurnRound = 0;
  target_heading = 0;

  steerMotor.runTarget(4000, 0, MotorStop::HOLD);

  driveMotor.run(driveSpeed);
}

void loop() {
  updateHeading();
  int frontDist = tof_front.getDistance();
  int distanceRight = tof_right.getDistance();
  int distanceLeft = tof_left.getDistance();
  bool hasBlock = hl.requestBlocks(result) && (result.ID == 1 || result.ID == 2);
  float error = calculateHeadingError(heading, target_heading);
  int steer_correction = constrain((int)(error * Kp), -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
  long timenow = 0;
  int diff = 0;
   if(TurnRound ==0){
   if(distanceRight > 1000){
   directionmult = 1;
   }else if (distanceLeft > 1000){
   directionmult = -1;
   }}
   if(directionmult == 1){
   distanceAbs = distanceRight;
   distanceAbsNeg = distanceLeft;
   }else if(directionmult == -1){
   distanceAbs = distanceLeft;
   distanceAbsNeg = distanceRight;}
  driveMotor.run(driveSpeed);
  steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);
  if(TurnRound < 12){
    if(distanceAbs > 600 && driveMotor.getAngle() > 3000 && !isManeuvering) {
    evo.playTone(1500, 200);
    while (tof_front.getDistance() > 100) {
      updateHeading();
      float error = calculateHeadingError(heading, target_heading);
      int steer_correction = constrain((int)(error * Kp), -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
      driveMotor.run(driveSpeed);
      steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);
      evo.writeToDisplay("error", 0, 32);
      evo.writeToDisplay(error, 70, 32);
    }
    evo.playTone(1000, 200);
    driveMotor.brake();

    if (distanceAbsNeg > 300 && !isManeuvering) {
      evo.playTone(1500, 200);
      steerMotor.runTarget(driveSpeed, -1*MAX_STEER_ANGLE*directionmult, MotorStop::HOLD, false);
      driveMotor.runAngle(-driveSpeed, 1500);
      steerMotor.runTarget(driveSpeed, 0, MotorStop::HOLD, false);
    }

    else {
      evo.playTone(3000, 200);
      driveMotor.runAngle(-driveSpeed, 1400);
      steerMotor.runTarget(driveSpeed, MAX_STEER_ANGLE*directionmult, MotorStop::HOLD, false);
      driveMotor.runAngle(driveSpeed, 1500);
    }

    TurnRound += 1;
    if(directionmult == -1){
    RoundMod = 3*(TurnRound % 4);
    target_heading = (90 * RoundMod)%360;}
    else if (directionmult == 1){
    RoundMod = TurnRound % 4;
    target_heading = 90 * RoundMod ; 
    }
    distanceAbs = 0;
    distanceAbsNeg = 0;
    driveMotor.resetAngle();
    driveMotor.run(driveSpeed);
    turning = false;
    }

    if (hasBlock && !isManeuvering) {
    int screenCenter = 160;
    int visualError = result.xCenter - screenCenter;
    steer_correction = constrain(visualError / 2, -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
    steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);

      if (hasBlock && frontDist <= 200) {
        isManeuvering = true;
        driveMotor.brake();
        delay(100);
        // driveMotor.runAngle(-driveSpeed, 200);
        // delay(100);
        int steerAngle = (result.ID == 1) ? -50 : 50;
        steerMotor.runTarget(driveSpeed, steerAngle, MotorStop::HOLD, false);
        driveMotor.runAngle(driveSpeed, 400);
        steerMotor.runTarget(driveSpeed, 0, MotorStop::HOLD, false);
        driveMotor.runAngle(driveSpeed, 300);
        steerMotor.runTarget(driveSpeed, -steerAngle, MotorStop::HOLD, false);
        driveMotor.runAngle(driveSpeed, 600);
        steerMotor.runTarget(driveSpeed, 0, MotorStop::HOLD, false);
        isManeuvering = false;
        driveMotor.run(driveSpeed);
      } 
      else {
        driveMotor.run(driveSpeed);
      }
    }

    else if (!isManeuvering) {
    steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);
    driveMotor.run(driveSpeed);
    }
  }
  
  else{
    // while(tof_front.getDistance() > 100) {
    // updateHeading();
    // float error = calculateHeadingError(heading, target_heading);
    // int steer_correction = constrain((int)(error * Kp), -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
    // driveMotor.run(driveSpeed);
    // steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);
    // evo.writeToDisplay("error", 0, 32);
    // evo.writeToDisplay(error, 70, 32);}
    // if(distanceAbs>2000){
    // steerMotor.runTarget(4000, directionmult*MAX_STEER_ANGLE, MotorStop::HOLD, false);
    // driveMotor.runAngle(4000, 1100);
    // steerMotor.runTarget(4000, 0, MotorStop::HOLD, false);
    // TurnRound += 1;
    // RoundMod = TurnRound%4;
    // target_heading = directionmult*90*RoundMod;
    // } else{
    // driveMotor.runAngle(4000, -200);
    // steerMotor.runTarget(4000, directionmult*MAX_STEER_ANGLE, MotorStop::HOLD, false);
    // driveMotor.runAngle(4000, 1100);
    // steerMotor.runTarget(4000, 0, MotorStop::HOLD, false);
    // while(tof_front.getDistance() > 500){
    // driveMotor.run(4000);}
    // steerMotor.runTarget(4000, directionmult*30, MotorStop::HOLD, false);
    // driveMotor.runAngle(4000, -500);
    // steerMotor.runTarget(4000, -1*directionmult*30, MotorStop::HOLD, false);
    // driveMotor.runAngle(4000, -300);
    driveMotor.brake();
    steerMotor.brake();
    }
  evo.clearDisplay();
  evo.writeToDisplay("absnigger ", 0, 0);
  evo.writeToDisplay(distanceAbs, 50, 0);
  evo.writeToDisplay("abs ", 0, 16);
  evo.writeToDisplay(distanceAbsNeg, 50, 16);
  evo.writeToDisplay("gyro", 0, 32);
  evo.writeToDisplay(heading, 40, 32);
  evo.writeToDisplay("target", 0, 48);
  evo.writeToDisplay(target_heading, 40, 48);
  evo.drawDisplay();
}