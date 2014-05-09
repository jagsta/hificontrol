#include <Stepper.h>
#include <IRremote.h>

// IR receives on pin 7
IRrecv irrecv(7); 
decode_results results;
int last;

// change this to the number of steps on your motor
#define STEPS 200

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 8,9,10,11);

// the previous reading from the analog input

void setup()
{
  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(15);
  irrecv.enableIRIn(); // enable IR receiver
}

void loop()
{
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFA857) {
    //  Serial.println("DOWN");
      stepper.step(-1);
      last = 1;
    }
    else if (results.value == 0xFF906F) {
     // Serial.println("UP");
      stepper.step(1);
      last = 2;
    }
    else if (results.value == 0xFFFFFFFF) {
      Serial.print("REPEAT ");
      switch ( last ) {
        case 0:
         // Serial.println("IGNORE");
          break;
        case 1:
         // Serial.println("DOWN");
          stepper.step(-1);
          break;
        case 2:
         // Serial.println("UP");
          stepper.step(1);
          break;
      }
    }
    else {
      // Serial.println("IGNORE");
      last = 0;
    }
    irrecv.resume(); // Continue receiving
  }
}
