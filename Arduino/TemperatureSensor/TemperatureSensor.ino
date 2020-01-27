void setup() {
pinMode(A0,INPUT);
Serial.begin(9600);
}

void loop() {
  int temp = analogRead(A0);
Serial.print("Temperature = ");
Serial.print(temp);

delay(1000);
}
