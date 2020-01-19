#include <Stepper.h>
#include <SerialCommand.h>

// defines pins numbers
const int stepPin = 7; 
const int dirPin = 3; 
const int enPin = 9;

const int delay_usec = 200;


#define arduinoLED 13   // Arduino LED on board


SerialCommand sCmd;     // The demo SerialCommand object

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);

  digitalWrite(enPin,LOW);

   
  pinMode(arduinoLED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);    // default to LED off

  Serial.begin(9600);

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("ON",    LED_on);          // Turns LED on
  sCmd.addCommand("OFF",   LED_off);         // Turns LED off
  sCmd.addCommand("HELLO", sayHello);        // Echos the string argument back
  sCmd.addCommand("P",     processCommand);  // Converts two arguments to integers and echos them back
  sCmd.addCommand("MOVE",  processMove);  
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Ready");
  
}

//////////////////////////////////////////////////////////////////////////////////////////
void loop() {
   

  sCmd.readSerial();
 /* Move(3200, delay_usec);
  delay(1000);
  Move(-3200, delay_usec);
  delay(1000);
  */
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void Move(long nsteps){
  if(nsteps < 0){
    digitalWrite(dirPin,LOW); 
    nsteps = -nsteps;
    }
  else
    digitalWrite(dirPin,HIGH); 
  
  for(long i = 0; i < nsteps; i++){
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(delay_usec); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(delay_usec); 
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
void LED_on() {
  Serial.println("LED on");
  digitalWrite(arduinoLED, HIGH);
}

//////////////////////////////////////////////////////////////////////////////////////////
void LED_off() {
  Serial.println("LED off");
  digitalWrite(arduinoLED, LOW);
}

//////////////////////////////////////////////////////////////////////////////////////////
void sayHello() {
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    Serial.print("Hello ");
    Serial.println(arg);
  }
  else {
    Serial.println("Hello, whoever you are");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
void processCommand() {
  int aNumber;
  char *arg;

  Serial.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    Serial.print("First argument was: ");
    Serial.print(aNumber);
  }
  else {
    Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atol(arg);
    Serial.print("Second argument was: ");
    Serial.println(aNumber);
  }
  else {
    Serial.println("No second argument");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
void processMove() {
  long nsteps;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    nsteps = atol(arg);    // Converts a char string to an integer
    
    Serial.print("Moving ");
    Serial.print(nsteps);
    Serial.println(" steps");

    
    Move(nsteps);
  }
  else {
    Serial.println("ERR");
    Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    Serial.println("ERR");
    Serial.println("too many arguments");
  }

  
  Serial.println("DONE");

}

//////////////////////////////////////////////////////////////////////////////////////////
// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("ERR");
  Serial.print("Command unrecognized: ");
  Serial.println(command);
}
