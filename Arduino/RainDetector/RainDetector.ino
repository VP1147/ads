// Declare I/O pins
int Sensor = A0;
int motor = 3;

// Declare global variables
int bg;
long int meter;
bool IsClosed;

void setup() {
  Serial.begin(9600);
  pinMode(motor, OUTPUT);
  pinMode(Sensor, INPUT);
  bg = calibrate(Sensor);
  //digitalWrite(motor, LOW);
}

void loop() {
  int Read = abs(analogRead(Sensor)-bg);
  delay(25);
  Serial.print("\tREAD: "); Serial.print(Read); Serial.print("\t\t");
  Serial.print("MTR: "); Serial.print(meter); Serial.print("\t\t");
  Serial.print("CLSD: "); Serial.print(IsClosed); Serial.print("\t\t");
  Serial.print("\n");
  if((abs(analogRead(Sensor)-bg)-Read > 15) && meter < 8192) {
    Serial.println(">> READ");
    meter += 60;
  }
  if(meter > 0) { 
    if(meter > 200 && IsClosed == true) { open(motor); IsClosed = false; }
    meter--;
  }
  if(meter == 0 && IsClosed == false) { close(motor); IsClosed = true; }
}

int calibrate(int pin) {
  // Starts the sensor calibration cycle
  // Called on setup, runs once
  Serial.println(" >>> Calibrating <<< ");
  long int sum = 0;
  for(int i=0; i<100; i++){
    sum += analogRead(pin);
    delay(50);
  }
  int bg = sum/100;
  Serial.println(" >>> Calibrated <<<");
  Serial.println(" >>> Background value set to "+String(bg)+" <<< ");
  delay(2000);
  return bg;
}

void open(int pin) {
  // Starts the opening cycle
  Serial.println(">> Opening ...");

  // Work in progress
  // For now this function only turns on a LED connected to motor port
  for(int i = 0; i < 256; i++) { analogWrite(pin, i); delay(2); }
}

void close(int pin) {
  // Starts the closing cycle
  Serial.println(">> Closing ...");

  // Work in progress
  // For now this function only turns off a LED connected to the motor port
  for(int i = 255; i > 0; i--) { analogWrite(pin, i); delay(2); }
}
