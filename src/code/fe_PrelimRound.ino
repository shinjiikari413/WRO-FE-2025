#include <Evo.h>
#include <SoftwareSerial.h>

EVOX1 evo;
EvoBNO055 bno(I2C3);

EvoVL53L0X tof_left(I2C1);
EvoVL53L0X tof_right(I2C2);

EvoMotor driveMotor(M2, EV3MediumMotor, false);
EvoMotor steerMotor(M3, EV3MediumMotor, true);

// IMU variables
float x, y, z;
float heading;
float target_heading = 0;
int TurnRound = 0;
int RoundMod = 0;

const float Kp = 2.1;
const int MAX_STEER_ANGLE = 30;
const int jumpThreshold = 45;  // Distance jump to trigger turn
const int driveSpeed = 4000;

int lastLeftDistance;
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
  Serial0.begin(115200);
  evo.begin();
  bno.begin();
  steerMotor.begin();
  driveMotor.begin();
  tof_left.begin();
  tof_right.begin();

  steerMotor.resetAngle();
  driveMotor.resetAngle();

  evo.writeLineToDisplay("Gyro + Wall Tracking", 0, true, true);
  delay(1000);
  evo.waitForPress();

  updateHeading();
  TurnRound = 0;
  target_heading = 0;

  steerMotor.runTarget(4000, 0, MotorStop::HOLD);
  driveMotor.run(driveSpeed);

  lastLeftDistance = tof_left.getDistance();
}

long timenow = 0;
int diff = 0;
int distanceLeft;
int distanceRight;
void loop() {
  updateHeading();
  float error = calculateHeadingError(heading, target_heading);
  int steer_correction = constrain((int)(error * Kp), -MAX_STEER_ANGLE, MAX_STEER_ANGLE);
  driveMotor.run(driveSpeed);
  steerMotor.runTarget(4000, steer_correction, MotorStop::HOLD, false);


  // every 25 millins 
  if ((millis() - timenow) > 25) {
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
    driveMotor.runAngle(4000, 100);
    steerMotor.runTarget(4000, -40, MotorStop::HOLD, false);
    driveMotor.runAngle(4000, 1100);
    steerMotor.runTarget(4000, 0, MotorStop::HOLD, false);
    TurnRound += 1;
    RoundMod = TurnRound%4;
    target_heading = -90*RoundMod;
    driveMotor.coast();
    delay(100);
    lastLeftDistance = tof_left.getDistance();
    driveMotor.resetAngle();
  }

  evo.clearDisplay();
  evo.writeToDisplay("L:", 0, 0);
  evo.writeToDisplay(distanceLeft, 30, 0);
  evo.writeToDisplay("R:", 0, 16);
  evo.writeToDisplay(distanceRight, 30, 16);
  evo.writeToDisplay("H:", 0, 32);
  evo.writeToDisplay((int)heading, 30, 32);
  evo.writeToDisplay("T:", 70, 32);
  evo.writeToDisplay((int)target_heading, 100, 32);
  evo.drawDisplay();
}