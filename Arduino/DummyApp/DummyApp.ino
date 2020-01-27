
int SpeedValue = 0;
int SecondCount = 0;
//int ButtonToggle = 0;

void setup() 
{
  // Initialize the serial port
  Serial.begin(9600);
}

void loop() 
{
  SecondCount++;

  if(SecondCount%5 == 0 )
  {
    SpeedValue++; 
    Serial.print(SpeedValue);
  }
  if(SecondCount > 35)
  { 
    Serial.print("B");
    SecondCount = 0;
    SpeedValue = 0;
  }
  // Loop Delay
  delay(2000);
}
