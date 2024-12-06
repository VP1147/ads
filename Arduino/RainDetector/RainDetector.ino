/*
  "Rain Detector"
  Arduino microcontroler code for ADS -
  Atmospheric Deposition Sampler).
  Instructions, circuits and documentation are
  avaliable on https://github.com/vp1147/ads.

  MIT License (See LICENSE for more information)
  Copyright (c) 2022 Vinícius Pavão

  LCA (Atmospheric Sciences Laboratory)
  INFI (Institute of Physics)
  UFMS (Federal University of Mato Grosso do Sul)
*/

// Enviromment variables
#define NONSTOP         false                // Motor Always on (only for debug-adjustemment).
                                             // May cause overheating!
#define SERIALRETURN    true                // Send verbose variable data to serial (debug or adjustemment).

#define IGNOREMS        true

// Defining I/O pins
int Sensor = A0;                             // Infrared Sensor (input)
int nmotor = 5;                              // Close signal pin (output)
int pmotor = 6;                              // Open signal pin (output)

// Microswitch pins - Detects if the lid is fully open or fully closed before stopping the motor
int ms_fullopen = 8;      // Open switch (input)
int ms_fullclose = 9;     // Closed switch (input)

// Information LEDs
int halt_led = 7; // RED
int op_cl_led = 10; // YELLOW
int det_led = 11; // GREEN

// Declaring global variables
const long interval = 10000;
unsigned long previousMillis = 0;
long int meter;
bool IsClosed = true; // Cycle starts closed
int counter = 0;
String input;         // Stores serial input text
// Defining sensor parameters
int ceiling = 400 ;           // Max. value for sensor counter - higher means
                              // slower response to rain <stopping>
int activate = 60 ;          // Value in which the opening signal is sent - higher
                              // means slower response to rain <starting>

void setup() {
  // Serial -> Input/Output  
  Serial.begin(9600);
  Serial.println(">> Starting");

  // Setting motor pins
  pinMode(pmotor, OUTPUT); pinMode(nmotor, OUTPUT);

  // Send HH to Relay - motor poweroff
  digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);

  // Setting IR sensor pin
  pinMode(Sensor, INPUT);

  pinMode(nmotor, OUTPUT);
  pinMode(pmotor, OUTPUT);

  pinMode(halt_led, OUTPUT);
  pinMode(op_cl_led, OUTPUT);
  pinMode(det_led, OUTPUT);

  pinMode(ms_fullopen, INPUT);
  pinMode(ms_fullclose, INPUT);
  
  if(VerifyState() == 'o') { close(); }
}

void loop() {
  if(NONSTOP == true) { digitalWrite(pmotor, LOW); digitalWrite(nmotor, HIGH); }
  else {                      // Normal operation
    digitalWrite(det_led, LOW);
    digitalWrite(20, LOW);
    int Read = abs(analogRead(Sensor));
    delay(30);
    if(SERIALRETURN == true) {
      // Output sensor parameters to serial
      Serial.print("\tREAD: "); Serial.print(Read); Serial.print("\t\t");
      Serial.print("METER: "); Serial.print(meter); Serial.print("\t\t");
      Serial.print("CLOSE: "); Serial.print(IsClosed); Serial.print("\t\t");
      Serial.print("MS_OPN: "); Serial.print(digitalRead(ms_fullopen)); Serial.print("\t\t");
      Serial.print("MS_CLS: "); Serial.print(digitalRead(ms_fullclose)); Serial.print("\t\t");
      Serial.print("\n");
    }

    // Check if diference between readings exceeds 15/1024
    // on the 20ms interval. Also if the meter value doesnt
    // exceed the wall limiter.
    if((abs(analogRead(Sensor)-Read) > 10) && meter < ceiling) {
      meter += 40;
      digitalWrite(det_led, HIGH);
      digitalWrite(14, HIGH);
    }
    // Starts opening cycle if meter reacher the activation value
    if(meter > 0) { 
      if(meter > activate && IsClosed == true) { open(); }
      // If open, subtracts from meter value until it reaches zero
      meter-- ;
    }
    // if meter value reaches one, starts closing cycle
    if(meter == 1 && IsClosed == false) { close(); }


    counter++ ;
    
  // Receive serial commands  
    if(Serial.available()) {
      input = Serial.readStringUntil('\n');
      if(input == "open") { open(); }
      else if(input == "close") { close(); }
      else { Serial.println(">> Unknown command: "+input); }
    }
  }

  // Measure state from microswitches
  if(VerifyState() == 'o') {
    IsClosed == false;
  }
  else if(VerifyState() == 'c') {
    IsClosed == true;
  }
}

void open() {
  if(VerifyState() == 'c') {
    // Starts the opening cycle
    Serial.println(">> Opening ...");

    // Wait until lid is fully open
    while(digitalRead(ms_fullopen) == LOW) {
      digitalWrite(pmotor, LOW); digitalWrite(nmotor, HIGH);
    }
  
    // Send HH to Relay - motor poweroff
    digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);
    Serial.println(">> Lid is fully open");
    IsClosed = false;
  }
  else if(VerifyState() == 'o') {
    Serial.println(">> OPENING CYCLE STOPPED - LID IS OPEN");
  }
  else if(VerifyState() == 'e') {
    Serial.println(">> OPENING CYCLE STOPPED - SYSTEM IS HALTED");
  }
}

// Verifies microswitch state
char VerifyState() {
  if(digitalRead(ms_fullclose) == HIGH && digitalRead(ms_fullopen) == LOW) {
    return 'c';
  }
  else if(digitalRead(ms_fullclose) == LOW && digitalRead(ms_fullopen) == HIGH) {
    return 'o';
  }
  else {
    Serial.println(">> ERROR - COULD NOT DETECT ANY MS SIGNAL!");
    Serial.println(">> SYSTEM MUST BE HALTED FOR PREVINING DAMAGE");
    return 'e';
  }
}
void close() {
  if(VerifyState() == 'o') {
    // Starts the closing cycle
    Serial.println(">> Closing ...");
    
    while(digitalRead(ms_fullclose) == LOW) {
      digitalWrite(nmotor, LOW); digitalWrite(pmotor, HIGH);
    }
    // Send HH to Relay - motor poweroff
    digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);
    Serial.println(">> Lid is fully closed");
    IsClosed = true;
  }
  else if(VerifyState() == 'c') {
    Serial.println(">> CLOSING CYCLE STOPPED - LID IS CLOSED");
  }
  else if(VerifyState() == 'e') {
    Serial.println(">> CLOSING CYCLE STOPPED - SYSTEM IS HALTED");
  }
}
