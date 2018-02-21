#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper(1, 11, 12); // 1 = Driver Mode, Step = pin 11, Direction = pin 12

void setup()
{ 
stepper.setMaxSpeed(3000.0);
stepper.setAcceleration(3000.0);
}

void loop()
{ 
stepper.runToNewPosition(0);
stepper.runToNewPosition(5000);
stepper.runToNewPosition(1000);
stepper.runToNewPosition(1200);
}

