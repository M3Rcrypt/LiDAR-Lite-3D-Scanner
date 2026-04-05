#include <Servo.h>
#include <LIDARLite.h>
#include <LiquidCrystal.h>

Servo servoX;
Servo servoY;
LIDARLite lidar;

// Minimum and maximum servo angle in degrees
// Modify to avoid hitting limits imposed by pan/tilt bracket geometry
int minPosX = 0;
int maxPosX = 180;
int minPosY = 40;
int maxPosY = 160;


int lastPosX = 0;
int lastPosY = 0;
int loopCount = 0;
int radius = 0;
int lastRadius = 0;

boolean scanning = true;
boolean scanDirection = true;
int scanIncrement = 1;
int posX = minPosX;
int posY = minPosY;
float pi = 3.14159265;
float deg2rad = pi / 180.0;

void setup() {


  lidar.begin(0, true);
  lidar.configure(0);
  servoX.attach(3);
  servoY.attach(2);
  servoX.write(posX);
  servoY.write(posY);
  Serial.begin(9600);
}

void loop() {
  if (scanning) {

    if (scanDirection) {
      posX += scanIncrement;
    } else {
      posX -= scanIncrement;
    }
    if (posX > maxPosX || posX < minPosX) {
      // hit limit X limit, reverse auto scan direction
      scanDirection = !scanDirection;
      posY += scanIncrement;
      if (posY > maxPosY) {
        // completed scan, reset and start again
        while (true) {
          delay(1000);
        }
      }
    }
  } 

  posX = min(max(posX, minPosX), maxPosX);
  posY = min(max(posY, minPosY), maxPosY);
  bool moved = moveServos();

  
  
  
  loopCount += 1;
  if (loopCount % 100 == 0) {
    // recalibrate scanner every 100 scans
    radius = lidar.distance();
  } else {
    radius = lidar.distance(false);
  }
  if (abs(radius - lastRadius) > 2)
  {
    lastRadius = radius;

  }
  if (scanning || moved) {
    float azimuth = posX * deg2rad;
    float elevation = (180 - maxPosY + posY) * deg2rad;
    double x = radius * sin(elevation) * cos(azimuth);
    double y = radius * sin(elevation) * sin(azimuth);
    double z = radius * cos(elevation);
    Serial.println(String(-x, 5) + " " + String(y, 5) + " " + String(-z, 5));
  }
    
  
}

bool moveServos()
{
  bool moved = false;
  static int lastPosX;
  static int lastPosY;
  int delta = 0;  
  if (posX != lastPosX) {
    delta += abs(posX - lastPosX);
    servoX.write(posX);
    lastPosX = posX;
    moved = true;
  }
  if (posY != lastPosY) {
    delta += abs(posY - lastPosY);
    servoY.write(posY);
    lastPosY = posY;
    moved = true;
  }
  
  delay(30);
  return moved;
}
