
// This is the latest file which is working with SDK Demo.

int button = 16; // push button is connected
int temp = 0;    // temporary variable for reading the button pin status

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

int sensorValue = 0;  // value read from the pot
int outputValue = 0;  // value to output to a PWM pin

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 // pinMode(button, INPUT); // declare push button as input
}

void loop() {
  // put your main code here, to run repeatedly:
sensorValue = analogRead(analogInPin);


  // 146
  // 292
  // 438
  // 584
  // 730
  // 876
  // 1022

if( sensorValue < 146)
  outputValue = 1; 
else if ( sensorValue < 292)
  outputValue = 2; 
else if ( sensorValue < 438)
  outputValue = 3; 
else if ( sensorValue < 584)
  outputValue = 4; 
else if ( sensorValue < 730)
  outputValue = 5; 
else if ( sensorValue < 876)
  outputValue = 6; 
else if ( sensorValue < 1022)
  outputValue = 7;
   
 // Serial.print(sensorValue);
  Serial.println(outputValue);

  delay(2000);
}
