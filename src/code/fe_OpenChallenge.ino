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

const float Kp = 2.1;
const int MAX_STEER_ANGLE = 30;
const int jumpThreshold = 45;  // Distance jump to trigger turn
const int driveSpeed = 3300;

int lastLeftDistance;
int lastRightDistance;
int MainDistance;
int focusDistance;
bool turning = false;

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

  delay(1000);
  evo.waitForBump();
  hl.clearScreen();


  updateHeading();
  TurnRound = 0;
  target_heading = 0;

  steerMotor.runTarget(4000, 0, MotorStop::HOLD);
  driveMotor.runTarget(driveSpeed, 100);

  lastLeftDistance = tof_left.getDistance();

  lastLeftDistance = tof_left.getDistance();
  lastRightDistance = tof_right.getDistance();
  if (lastLeftDistance > lastRightDistance){
    MainDistance = 1; //LEFT
  } 
  else if (lastLeftDistance < lastRightDistance){
    MainDistance = -1; //RIGHT
  }
  driveMotor.run(driveSpeed);
}

void loop() {
  updateHeading();
  int frontDist = tof_front.getDistance();
  bool hasBlock = hl.requestBlocks(result) && (result.ID == 1 || result.ID == 2);
  float error = calculateHeadingError(heading, target_heading);
  int steer_correction = constrain((int)(error * Kp), -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
  long timenow = 0;
  int diff = 0;
  int distanceLeft;
  int distanceRight;
  driveMotor.run(driveSpeed);
  steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);

  if ((millis() - timenow) > 10) {
    distanceLeft = tof_left.getDistance();
    distanceRight = tof_right.getDistance();
    Serial0.print("L: ");
    Serial0.print(distanceLeft);
    Serial0.print("  R: ");
    Serial0.println(distanceRight);
    timenow = millis();
  }

  if (distanceLeft > 1000 && driveMotor.getAngle() > 750) {
    turning = true;
    driveMotor.coast();
    delay(100);
    driveMotor.runAngle(driveSpeed, 100);
    steerMotor.runTarget(driveSpeed, -40, MotorStop::HOLD, false);
    driveMotor.runAngle(driveSpeed, 1100);
    steerMotor.runTarget(driveSpeed, 0, MotorStop::HOLD, false);
    TurnRound += 1;
    RoundMod = TurnRound % 4;
    target_heading = -90 * RoundMod;
    driveMotor.coast();
    delay(100);
    lastLeftDistance = tof_left.getDistance();
    driveMotor.resetAngle();
  }

  if (hasBlock) {
    int screenCenter = 160;
    int visualError = result.xCenter - screenCenter;
    steer_correction = constrain(visualError / 2, -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
    steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);

    if (frontDist <= 300 && hasBlock) {
    driveMotor.brake();
    delay(100);
    driveMotor.run(driveSpeed);

    int steerAngle = (result.ID == 1) ? 40 : -40;
    steerMotor.runTarget(driveSpeed, steerAngle, MotorStop::HOLD, false);
    driveMotor.run(driveSpeed);
    delay(500);

    steerMotor.runTarget(driveSpeed, 0, MotorStop::HOLD, false);
    delay(500);

    steerMotor.runTarget(driveSpeed, -steerAngle, MotorStop::HOLD, false);
    delay(500);
    steerMotor.runTarget(driveSpeed, 0, MotorStop::HOLD, false);
    driveMotor.run(driveSpeed);
    }

    else {
      driveMotor.run(driveSpeed);
    }
  }
  else {
    steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);
    driveMotor.run(driveSpeed); 
  }

  evo.clearDisplay();
  evo.writeToDisplay("block?: ", 0, 16);
  evo.writeToDisplay(hasBlock, 50, 16);
  evo.writeToDisplay("h", 0, 32);
  evo.writeToDisplay((int)heading, 70, 32);
  evo.writeToDisplay("front", 0, 48);
  evo.writeToDisplay(frontDist, 40, 48);
  evo.drawDisplay();

}
