// Define pins
#define clkPin 4 // CLK pin
#define dtPin 3  // DT pin
#define swPin 2  // SW pin

// Global variable to store encoder data
volatile long menuValue = 0;

// Previous states of pins
volatile int lastCLKState = 0;

volatile int setValues[4] = {20,80,60,50};

volatile bool settingValue = false;

// Encoder interrupt service routine for CLK
void readEncoder() {
  int clkState = digitalRead(clkPin);
  int dtState = digitalRead(dtPin);
  if(!settingValue) {
    if (clkState != lastCLKState && clkState == LOW) {
      if (dtState != clkState) {
        menuValue++;
      } else {
        menuValue--;
      }
    }
  }
  if(settingValue) {
    if (clkState != lastCLKState && clkState == LOW) {
      if (dtState != clkState) {
        setValues[menuValue] = setValues[menuValue] + 1;
      } else {
        setValues[menuValue] = setValues[menuValue] - 1;
      }
    }
  }
  lastCLKState = clkState;
}

// Button interrupt service routine for SW
void readButton() {
  Serial.println("Button pressed!");
  settingValue = !settingValue;
  Serial.println(settingValue);// Your button press handling code here
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(dtPin), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(swPin), readButton, FALLING);
}

void loop() {
  // Print encoder value for debugging
  Serial.print("menuValue:  ");
  Serial.println(menuValue);
  Serial.print("temperatureSet:  ");
  Serial.println(setValues[0]);
  Serial.print("humiditySet:  ");
  Serial.println(setValues[1]);
  Serial.print("soilMoistureSet:  ");
  Serial.println(setValues[2]);
  Serial.print("Grow LED's:  ");
  Serial.println(setValues[3]);
  delay(1000);
}
