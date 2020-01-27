
const int buttonPin = D0;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  analogRead(A0);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    Serial.print("Button Pressed \n");
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    Serial.print(analogRead(A0));
    Serial.print("test \n");
    digitalWrite(ledPin, LOW);
  }
  delay(1000);
}
