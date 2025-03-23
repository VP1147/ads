/*
  "Rain Detector - Simple"
  Simple version of the Arduino microcontroler 
  code for ADS - Atmospheric Deposition Sampler).
  Instructions, circuits and documentation are
  avaliable on https://github.com/vp1147/ads.

  MIT License (See LICENSE for more information)
  Copyright (c) 2022 Vinícius Pavão

  LCA (Atmospheric Sciences Laboratory)
  INFI (Institute of Physics)
  UFMS (Federal University of Mato Grosso do Sul)
*/

// Enviromment variables
#define SERIALRETURN    true                // Send verbose variable data to serial (debug or adjustemment).

// Defining I/O pins
int Sensor = 3;         // Digital rain sensor (input)
int nmotor = 5;         // Close signal pin (output)
int pmotor = 6;         // Open signal pin (output)

// Microswitch pins - Detects if the lid is fully open or fully closed
int ms_fullopen = 8;      // Open switch (input)
int ms_fullclose = 9;     // Closed switch (input)
int open_button = 7;      // External button

// Information LEDs (optional)
int halt_led = 7; // RED
int op_cl_led = 10; // YELLOW
int det_led = 11; // GREEN

// Declaring global variables
long int meter = 0;
int Read = 0;
bool IsClosed = true; // Cycle starts closed
int counter = 0;
String input;         // Stores serial input text
int timer=0;

void setup() {
  // Serial -> Input/Output  
  Serial.begin(9600);
  Serial.println(">> Starting");

  // Setting motor pins
  pinMode(pmotor, OUTPUT); pinMode(nmotor, OUTPUT);

  // Send HH to Relay - motor poweroff
  digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);

  // Setting sensor pin
  pinMode(Sensor, INPUT);

  pinMode(nmotor, OUTPUT);
  pinMode(pmotor, OUTPUT);

  pinMode(halt_led, OUTPUT);
  pinMode(op_cl_led, OUTPUT);
  pinMode(det_led, OUTPUT);

  pinMode(ms_fullopen, INPUT);
  pinMode(ms_fullclose, INPUT);
  
  if(VerifyState() == 'o') { close(true); }
  else if(VerifyState() == 'e') { close(false); }
}

void loop() {

  digitalWrite(det_led, LOW);
  delay(30);
  if(SERIALRETURN == true) {
    // Output sensor parameters to serial
    Serial.print("\tREAD: "); Serial.print(digitalRead(Sensor)); Serial.print("\t\t");
    Serial.print("CLOSE: "); Serial.print(IsClosed); Serial.print("\t\t");
    Serial.print("MS_OPN: "); Serial.print(digitalRead(ms_fullopen)); Serial.print("\t");
    Serial.print("MS_CLS: "); Serial.print(digitalRead(ms_fullclose)); Serial.print("\t");
    Serial.print("BTN: "); Serial.print(digitalRead(open_button)); Serial.print("\t");
    Serial.print("\n");
    }

  // Check if sensor pin is high (raining) and if the lid is closed
  if((digitalRead(Sensor) == LOW && IsClosed == true)
  || (digitalRead(open_button) == HIGH && IsClosed == true)) { open(false); }

  // Else, check if the sensor is low (not raining), lid is closed and the external button
  // is not pressed
  else if((digitalRead(Sensor) == HIGH && IsClosed == false)
  || (digitalRead(open_button) == LOW && IsClosed == false)) { close(false); }

  // Receive serial commands  
  if(Serial.available()) {
    input = Serial.readStringUntil('\n');
    if(input == "open") { open(true); }
    else if(input == "close") { close(true); }
    else if(input == "state") {
      Serial.print("\tREAD: "); Serial.print(digitalRead(Sensor)); Serial.print("\t\t");
      Serial.print("CLOSE: "); Serial.print(IsClosed); Serial.print("\t\t");
      Serial.print("MS_OPN: "); Serial.print(digitalRead(ms_fullopen)); Serial.print("\t");
      Serial.print("MS_CLS: "); Serial.print(digitalRead(ms_fullclose)); Serial.print("\t");
      Serial.print("BTN: "); Serial.print(digitalRead(open_button)); Serial.print("\t");
    }
    else { Serial.println(">> Unknown command: "+input); }
  }

  // Also open if the external button is pressed
  if(digitalRead(open_button) == HIGH && IsClosed == true) { open(true); }

  // Verify the lid state from microswitches
  if(VerifyState() == 'o') {
    IsClosed == false;
  }
  else if(VerifyState() == 'c') {
    IsClosed == true;
  }
}

int calibrate(int port, int steps, int period) {   // i.e. calibrate(Sensor, 100, 5000)
  int tot=0;
  for(int i=0; i<steps; i++) {
    tot+=analogRead(port);
    delay(int(period/steps));
  }
  return tot/steps;
}

void open(bool verify_closed) {
  if(VerifyState() == 'c' || verify_closed == false) {
    // Starts the opening cycle
    Serial.println(">> Opening ...");

    // Wait until lid is fully open
    timer = 0;
    while(digitalRead(ms_fullopen) == LOW || timer == 4000) {
      digitalWrite(pmotor, LOW); digitalWrite(nmotor, HIGH);
      delay(1);
      timer++;
    }
    delay(100);
    // Send HH to Relay - motor poweroff
    digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);
    Serial.println(">> Lid is fully open");
    IsClosed = false;
  }
  else if(VerifyState() == 'o' && verify_closed == true) {
    Serial.println(">> OPENING CYCLE STOPPED - LID IS OPEN");
  }
  else if(VerifyState() == 'e' && verify_closed == true) {
    Serial.println(">> OPENING CYCLE STOPPED - SYSTEM IS HALTED");
  }
}

// Verifies microswitch state
char VerifyState() {
  if(digitalRead(ms_fullclose) == HIGH) {
    return 'c';
  }
  else if(digitalRead(ms_fullopen) == HIGH) {
    return 'o';
  }
  else {
    Serial.println(">> ERROR - COULD NOT DETECT ANY MS SIGNAL!");
    return 'e';
  }
}
void close(bool verify_open) {
  if(VerifyState() == 'o' || verify_open == false) {
    // Starts the closing cycle
    Serial.println(">> Closing ...");
    timer = 0;
    while(digitalRead(ms_fullclose) == LOW || timer == 3000) {
      digitalWrite(nmotor, LOW); digitalWrite(pmotor, HIGH);
      delay(1);
      timer++;
    }
    // Send HH to Relay - motor poweroff
    digitalWrite(pmotor, HIGH); digitalWrite(nmotor, HIGH);
    Serial.println(">> Lid is fully closed");
    IsClosed = true;
  }
  else if(VerifyState() == 'c' || verify_open == true) {
    Serial.println(">> CLOSING CYCLE STOPPED - LID IS CLOSED");
  }
  else if(VerifyState() == 'e' || verify_open == true) {
    Serial.println(">> CLOSING CYCLE STOPPED - SYSTEM IS HALTED");
  }
}
