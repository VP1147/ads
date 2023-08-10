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

// Defining I/O pins
int Sensor = A0;                             // Infrared Sensor
int nmotor = 5;                              // Close signal pin 
int pmotor = 6;                              // Open signal pin

// Microswitch pins - Detects if the lid is fully open or fully closed before stopping the motor
int ms_fullopen = 8;
int ms_fullclose = 9;

// Declaring global variables
const long interval = 10000;
unsigned long previousMillis = 0;
long int meter;
bool IsClosed = true; // Cycle starts closed
int counter = 0;
String input;

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
}

void loop() {
  if(NONSTOP == true) { digitalWrite(pmotor, LOW); digitalWrite(nmotor, HIGH); }
  else {
    int Read = abs(analogRead(Sensor));
    delay(30);
    if(SERIALRETURN == true) {
      // Output sensor parameters to serial
      Serial.print("\tREAD: "); Serial.print(Read); Serial.print("\t\t");
      Serial.print("MTR: "); Serial.print(meter); Serial.print("\t\t");
      Serial.print("CLSD: "); Serial.print(IsClosed); Serial.print("\t\t");
      Serial.print("MS_OPN: "); Serial.print(digitalRead(ms_fullopen)); Serial.print("\t\t");
      Serial.print("MS_CLS: "); Serial.print(digitalRead(ms_fullclose)); Serial.print("\t\t");
      Serial.print("\n");
    }

    // Check if diference between readings exceeds 15pwm
    // on the 20ms interval. Also if the meter value doesnt
    // exceed the wall limiter.
    if((abs(analogRead(Sensor)-Read) > 15) && meter < 600) {
      meter += 25;
    }
    // Starts opening cycle if meter value reaches 150
    if(meter > 0) { 
      if(meter > 150 && IsClosed == true) { open(); }
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
}

void open() {
  // Starts the opening cycle
  Serial.println(">> Opening ...");
  Serial.println(">> Waiting for MS detection to stop");

  // Wait until lid is fully open
  while(digitalRead(ms_fullopen) == false) {
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, HIGH);
  }
  
  // Send HH to Relay - motor poweroff
  Serial.println(">> MS detected - Stopped!");
  digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);
  IsClosed = false;
}

void close() {
  // Starts the closing cycle
  Serial.println(">> Closing ...");
  Serial.println(">> Waiting for MS detection to stop");

  while(digitalRead(ms_fullclose) == false) {
    digitalWrite(nmotor, LOW); digitalWrite(pmotor, HIGH);
  }
  // Send HH to Relay - motor poweroff
  Serial.println(">> MS detected - Stopped!");
  digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);
  IsClosed = true;
}
