/* Arduino hifi control
   Receives IR codes and either:
     rotates a stepper motor to control manual amp volume
     changes input on CA DACMagic via optoisolator

   The stepper is rubber belted to the volume control

   * TODO *
   - Move constant values to #defines
   - Invesitgate power saving on the arduino, it's going to spend a lot of the time doing nothing
   - Ethernet Shield and api to control via IP
   - LED to confirm known IR command received
   - use hardware mirostepping controller I bought at Leeds Hackspace

   - decide on remote commands to use for DAC inputs (need 3)
*/

#include <Stepper.h>
#include <IRremote.h>

// DAC I/O
#define selectPin 1
#define usbPin 2
#define in1Pin 3
#define in2Pin 4

// IR receives on pin 7
IRrecv irrecv(7); 
decode_results results;

// number of steps on your motor
#define STEPS 200

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 8,9,10,11);

int countdown = 10000;
int source = 0;

void setup()
{
  // DAC setup
  pinMode(selectPin,OUTPUT);
  digitalWrite(selectPin,LOW);
  pinMode(usbPin,INPUT);
  digitalWrite(usbPin,HIGH);
  pinMode(in1Pin,INPUT);
  digitalWrite(in1Pin,HIGH);
  pinMode(in2Pin,INPUT);
  digitalWrite(in2Pin,HIGH);
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
    // get the state of the DAC
    bool usbState = digitalRead(usbPin);
    bool in1State = digitalRead(in1Pin);
    bool in2State = digitalRead(in2Pin);
    // set the source accordingly (USB=1,In1=2,In2=3)
    // LOW is a selected input, there can only be one low input
    if (!usbState) {
      source = 1;
    }
    else if (!in1State) {
      source = 2;
    }
    else if (!in2State) {
      source = 3;
    };
    // Input USB code
    if (results.value == 0x2BC ) {
      // input 1
      selectInput(source, 1);
    }
    if (results.value == 0x2BD ) {
      // input 2
      selectInput(source, 2);
    }
    if (results.value == 0x2BE ) {
      // input 3
      selectInput(source, 3);
    }
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
      stepper.step(80);
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
};

void selectInput (int oldInput, int newInput) {
  int iters  = newInput - oldInput;
  if (iters<0) {
    iters += 3;
  }
  for(int i=0;i<iters;i++) {
    digitalWrite(selectPin,HIGH);
    delay(50);
    digitalWrite(selectPin,LOW);
    delay(100);
  }
}
