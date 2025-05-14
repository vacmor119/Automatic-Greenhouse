// Define pins
#define clkPin 4 // CLK pin
#define dtPin 3  // DT pin
#define swPin 2  // SW pin

// Global variable to store encoder data
volatile long menuValue = 0;

// Previous states of pins
volatile int lastCLKState = 0;

volatile float temperatureSet = 0;
volatile float humiditySet = 0;
volatile float soilMoistureSet = 0;
volatile bool growLedSet = false;

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
        switch (menuValue) {
          case 1:
            temperatureSet = temperatureSet + 0.1;
            break;
          case 2:
            humiditySet = humiditySet + 0.1;
            break;
          case 3:
            soilMoistureSet = soilMoistureSet + 0.1;
            break;
          case 4:
            growLedSet = true;
            break;
        }
      } else {
        switch (menuValue) {
          case 1:
            temperatureSet = temperatureSet - 0.1;
            break;
          case 2:
            humiditySet = humiditySet - 0.1;
            break;
          case 3:
            soilMoistureSet = soilMoistureSet - 0.1;
            break;
          case 4:
            growLedSet = false;
            break;
        }
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
  Serial.println(temperatureSet);
  Serial.print("humiditySet:  ");
  Serial.println(humiditySet);
  Serial.print("soilMoistureSet:  ");
  Serial.println(soilMoistureSet);
  Serial.print("Grow LED's:  ");
  Serial.println(growLedSet);
  delay(1000);
}
