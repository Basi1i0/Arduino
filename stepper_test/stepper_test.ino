
#include <SerialCommand.h>

//
struct stepper_t
{
  const int stepPin;
  const int dirPin;
  const int enPin;
  unsigned int delayUsec;

  long currentPos;
  long startLim;
  long endLim;
};

// defines pins numbers
const int stepPin = 7; 
const int dirPin = 3; 
const int enPin = 9;

const int delay_usec = 200;


stepper_t stepperMotor = {.stepPin = 7, .dirPin = 3, .enPin = 9, .delayUsec = 200,
                          .currentPos = 0, .startLim = 0, .endLim = 120000};

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
  sCmd.addCommand("MOVETO",  processMoveTo);  
  sCmd.addCommand("POS",  SendCurrentPos);  
  sCmd.addCommand("END",  SendEndLim);  
  sCmd.addCommand("START",  SendStartLim);
  sCmd.addCommand("SETEND",  SetEndLim);  
  sCmd.addCommand("SETSTART",  SetStartLim);  
  sCmd.addCommand("MOVE",  processMove);  
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Ready");
  
}

//////////////////////////////////////////////////////////////////////////////////////////
void loop() {
   

  sCmd.readSerial();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void Move(long nsteps){

  if(stepperMotor.currentPos + nsteps > stepperMotor.endLim ||
     stepperMotor.currentPos + nsteps < stepperMotor.startLim){
    return;
  }
  
  stepperMotor.currentPos += nsteps;
  
  if(nsteps < 0){
    digitalWrite(stepperMotor.dirPin,LOW); 
    nsteps = -nsteps;
    }
  else
    digitalWrite(dirPin,HIGH); 
  
  for(long i = 0; i < nsteps; i++){
    digitalWrite(stepperMotor.stepPin,HIGH); 
    delayMicroseconds(stepperMotor.delayUsec); 
    digitalWrite(stepperMotor.stepPin,LOW); 
    delayMicroseconds(stepperMotor.delayUsec); 
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
void SendCurrentPos() {
  Serial.println(stepperMotor.currentPos);
}

void SendEndLim() {
  Serial.println(stepperMotor.endLim);
}

void SendStartLim() {
  Serial.println(stepperMotor.startLim);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SetStartLim() {
  long newStartLim;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    newStartLim = atol(arg);    // Converts a char string to an integer
    if (newStartLim < stepperMotor.endLim)
      stepperMotor.startLim = newStartLim;
    else{
      Serial.println("ERR");
      Serial.println("START should be smaller than END");
      }
  }
  else {
    stepperMotor.startLim = stepperMotor.currentPos;
  }
  
  
  Serial.print("startLim is ");
  Serial.println(stepperMotor.startLim);
  Serial.println("DONE");
}

//////////////////////////////////////////////////////////////////////////////////////////
void SetEndLim() {
  long newEndLim;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    newEndLim = atol(arg);    // Converts a char string to an integer
    if (newEndLim > stepperMotor.endLim)
      stepperMotor.endLim = newEndLim;
    else{
      Serial.println("ERR");
      Serial.println("END should be greater than START");
    }
  }
  else {
    stepperMotor.endLim = stepperMotor.currentPos;
  }
  
  
  Serial.print("endLim is ");
  Serial.println(stepperMotor.endLim);
  Serial.println("DONE");
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

    if(stepperMotor.currentPos + nsteps > stepperMotor.endLim ||
       stepperMotor.currentPos + nsteps < stepperMotor.startLim){
      Serial.println("ERR");
      Serial.println("Result should be within limits");
    }
    else{
      Serial.print("Moving ");
      Serial.print(nsteps);
      Serial.println(" steps");
  
      Move(nsteps);
    }
  }
  else {
    Serial.println("ERR");
    Serial.println("No arguments");
  }
  
  Serial.print("Current position ");
  Serial.println(stepperMotor.currentPos);
  Serial.println("DONE");
}


//////////////////////////////////////////////////////////////////////////////////////////
void processMoveTo() {
  long dest;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    dest = atol(arg);    // Converts a char string to an integer

    if(dest > stepperMotor.endLim ||
       dest < stepperMotor.startLim){
      Serial.println("ERR");
      Serial.println("Result should be within limits");
    }
    else{
      Serial.print("Moving to ");
      Serial.println(dest);
  
      Move(dest - stepperMotor.currentPos);
    }
  }
  else {
    Serial.println("ERR");
    Serial.println("No arguments");
  }
  
  Serial.print("Current position ");
  Serial.println(stepperMotor.currentPos);
  Serial.println("DONE");
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
// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("ERR");
  Serial.print("Command unrecognized: ");
  Serial.println(command);
}
