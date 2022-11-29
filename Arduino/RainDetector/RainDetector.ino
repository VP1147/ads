/*
  "Rain Detector" 
  Arduino microcontroler code for ADS -
  Atmospheric Deposition Sampler).
  Instructions, circuits and documentation are
  avaliable on https://github.com/vp1147/ads.

  MIT License (See LICENSE for more information)
  Copyright (c) 2022 Vinícius Pavão

  LCA (Atmospheric Sciences Laboratory) /
  UFMS (Federal University of Mato Grosso do Sul)
*/

// Including Adafruit DHT library
#include "DHT.h" 

// Defining DHT pin and type
#define DHTPIN A2
#define DHTTYPE DHT11

// TRUE - Use DHT sensor (experimental)
// FALSE - Ignore DHT sensor
#define DHTISUP false 

// Defining I/O pins
int Sensor = A0;
int nmotor = 5;
int pmotor = 6;

// Declaring global variables
long int meter;
bool IsClosed = true; // Cycle starts closed

// Calling DHT function w/ defined parameters
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Begin Serial output
  Serial.begin(9600);

  // Begin DHT  
  dht.begin();

  // Setting motor pins
  pinMode(pmotor, OUTPUT); pinMode(nmotor, OUTPUT);

  // Setting IR sensor pin
  pinMode(Sensor, INPUT);
}

void loop() {
  
  int Read = abs(analogRead(Sensor));
  delay(20);
  Serial.print("\tREAD: "); Serial.print(Read); Serial.print("\t\t");
  Serial.print("MTR: "); Serial.print(meter); Serial.print("\t\t");
  Serial.print("CLSD: "); Serial.print(IsClosed); Serial.print("\t\t");

  if(DHTISUP == true) {
    // Reading temperature and humidity from DHT sensor
    float humid = dht.readHumidity();
    float temp = dht.readTemperature();

    // Check for valid value, them print out on Serial
    if (! isnan(temp)) { Serial.print("TEMP: "); Serial.print(temp); Serial.print(" C\t\t"); } 
    if (! isnan(humid)) { Serial.print("HUMID: "); Serial.print(humid); Serial.print(" %\t\t"); }
  }
  Serial.print("\n");

  // Check if diference between readings exceeds 15pwm
  // on the 20ms interval. Also if the meter value doesnt
  // exceed the wall limiter.
  if((abs(analogRead(Sensor)-Read) > 15) && meter < 600) {
    meter += 25;
  }
  // Starts opening cycle if meter value reaches 150
  if(meter > 0) { 
    if(meter > 150 && IsClosed == true) { open(); IsClosed = false; }
    // If open, subtracts from meter value until it reaches zero
    meter--;
  }
  // if meter value reaches zero, starts closing cycle
  if(meter == 0 && IsClosed == false) { close(); IsClosed = true; }
}

void open() {
  // Starts the opening cycle
  Serial.println(">> Opening ...");

  //for(int i = 0; i < 256; i++) { analogWrite(pin, i); delay(2); }

  // Motor power interval (ms)
  int t = 3000;

  // Set N pin HIGH for `t` seconds
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, HIGH);
  delay(t);
  
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, LOW);
}

void close() {
  // Starts the closing cycle
  Serial.println(">> Closing ...");

  //for(int i = 255; i > 0; i--) { analogWrite(pin, i); delay(2); }

  // Motor power interval (ms)
  int t = 3000;

  // Set P pin HIGH for `t` seconds
  digitalWrite(nmotor, LOW); digitalWrite(pmotor, HIGH);
  delay(t);
  
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, LOW);
}
