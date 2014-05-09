/* Arduino volume controller
   Receives IR codes and rotates a stepper accordingly
   The stepper is rubber banded to the volume control
   Added a insulating tape belt over the rubber band to reduce stretch which was causing steps in volume

   * TODO *
   - Move constant values to #defines
   - Invesitgate power saving on the arduino, it's going to spend a lot of the time doing nothing
   - Ethernet Shield and api to control via IP
   - LED to confirm known IR command received
   - software microstepping using PWM outputs to H-bridge and lookup table for sin/cos value to reduce noise
     - PWM should be >20KHz (timers 1 and 2 with 0 multiplier would give ~30KHz
     - Use 8 element array of bytes for PWM values and index for the 2 phases ( 90 degree offset )
     - would involve abandoning/forking the stepper library and implementing myself
*/

#include <Stepper.h>
#include <IRremote.h>

// IR receives on pin 7
IRrecv irrecv(7); 
decode_results results;

// change this to the number of steps on your motor
#define STEPS 200

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 8,9,10,11);

int countdown = 10000;

void setup()
{
  // We use pins 5 and 6 for Vcc of the 2 sides of the H-bridge
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  // Apply 5V to the H-bridge
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  // initialise the volume by rotating to minimum then up to a sensible volume
  stepper.setSpeed(60);
  stepper.step(-540);
  stepper.step(100);
  stepper.setSpeed(20);
  // enable IR receiver
  irrecv.enableIRIn(); 
  Serial.begin(9600);
}

void loop()
{
  // if we have an IR event to check
  if (irrecv.decode(&results)) {
    // volume down code
    if (results.value == 0x217C346) {
    //  Serial.println("DOWN");
      // Set the Vcc high
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      // Step 2 steps anti-clockwise
      stepper.step(-2);
      // set the countdown timer
      countdown = 10000;
    }
    // volume up code
    else if (results.value == 0x6E283FF1) {
     // Serial.println("UP");
      // Set the Vcc high
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      // Step 2 steps clockwise
      stepper.step(2);
      // set the countdown timer
      countdown = 10000;
    }
    // initialise code
    else if (results.value == 0xDF2FAD9B) {
      // Serial.println("INITIALISE");
      // Set the Vcc high
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      // crank up the rpm a bit
      stepper.setSpeed(60);
      // turn it down
      stepper.step(-540);
      // back up a bit
      stepper.step(100);
      // set the rpm back to default
      stepper.setSpeed(20);
      // set the countdown timer
      countdown = 10000;
    }
    irrecv.resume(); // Continue receiving
  }
  // countdown 1 tick per ms
  countdown--;
  delay(1);
  // if countdown has expired
  if (countdown<1) {
    // Drop Vcc to H-bridge (idle the stepper)
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
  }
}
