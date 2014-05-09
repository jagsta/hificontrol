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

int countdown = 1000;

void setup()
{
  // set the speed of the motor to 30 RPMs
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  stepper.setSpeed(60);
  stepper.step(-540);
  stepper.step(100);
  stepper.setSpeed(20);
  irrecv.enableIRIn(); // enable IR receiver
}

void loop()
{
  if (irrecv.decode(&results)) {
    if (results.value == 0x217C346) {
    //  Serial.println("DOWN");
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      stepper.step(-2);
      countdown = 10000;
    }
    else if (results.value == 0x6E283FF1) {
     // Serial.println("UP");
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      stepper.step(2);
      countdown = 10000;
    }
    else if (results.value == 0xDF2FAD9B) {
      // Serial.println("INITIALISE");
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      stepper.setSpeed(60);
      stepper.step(-540);
      stepper.step(100);
      stepper.setSpeed(20);
      countdown = 10000;
    }
    irrecv.resume(); // Continue receiving
  }
  countdown--;
  delay(1);
  if (countdown<1) {
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
  }
}
