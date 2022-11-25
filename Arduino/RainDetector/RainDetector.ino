#include "DHT.h"
 
#define DHTPIN A2 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

// Declare I/O pins
int Sensor = A0;
int nmotor = 5;
int pmotor = 6;

// Declare global variables
long int meter;
bool IsClosed;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(pmotor, OUTPUT); pinMode(nmotor, OUTPUT);
  pinMode(Sensor, INPUT);
  //bg = calibrate(Sensor);
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, LOW);
}

void loop() {
  float humid = dht.readHumidity();
  float temp = dht.readTemperature();
  int Read = abs(analogRead(Sensor));
  delay(20);
  Serial.print("\tREAD: "); Serial.print(Read); Serial.print("\t\t");
  Serial.print("MTR: "); Serial.print(meter); Serial.print("\t\t");
  Serial.print("CLSD: "); Serial.print(IsClosed); Serial.print("\t\t");
  if (! isnan(temp)) { Serial.print("TEMP: "); Serial.print(temp); Serial.print(" C\t\t"); } 
  if (! isnan(humid)) { Serial.print("HUMID: "); Serial.print(humid); Serial.print(" %\t\t"); }
  Serial.print("\n");
  if((abs(analogRead(Sensor)-Read) > 15) && meter < 600) {
    meter += 25;
  }
  if(meter > 0) { 
    if(meter > 150 && IsClosed == true) { open(); IsClosed = false; }
    meter--;
  }
  if(meter == 0 && IsClosed == false) { close(); IsClosed = true; }
}

void open() {
  // Starts the opening cycle
  Serial.println(">> Opening ...");
  //delay(10);

  // Work in progress
  //for(int i = 0; i < 256; i++) { analogWrite(pin, i); delay(2); }

  int timer = 3000; // milliseconds
  digitalWrite(pmotor, LOW);
  digitalWrite(nmotor, HIGH);
  delay(timer);
  
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, LOW);
}

void close() {
  // Starts the closing cycle
  Serial.println(">> Closing ...");
  //delay(10);
  
  // Work in progress
  //for(int i = 255; i > 0; i--) { analogWrite(pin, i); delay(2); }

  int timer = 3000; // milliseconds
  digitalWrite(nmotor, LOW);
  digitalWrite(pmotor, HIGH);
  delay(timer);
  
  digitalWrite(pmotor, LOW); digitalWrite(nmotor, LOW);
}
