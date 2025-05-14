#define SW_PIN 2   // Push button pin connected to D2
#define CLK_PIN 3  // Clock pin connected to D3
#define DT_PIN 4   // Data pin connected to D4


volatile int encoder_counter = 0;  // Variable to store the encoder count
volatile int direction = 0;  // Variable to store the rotation direction
volatile bool buttonPressed = false;  // Variable to store button press state

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  pinMode(CLK_PIN, INPUT_PULLUP);  // Set CLK pin as input with pull-up resistor
  pinMode(DT_PIN, INPUT_PULLUP);   // Set DT pin as input with pull-up resistor
  pinMode(SW_PIN, INPUT_PULLUP);   // Set SW pin as input with pull-up resistor

  attachInterrupt(digitalPinToInterrupt(CLK_PIN), readEncoder, CHANGE);  // Attach interrupt to CLK pin
  attachInterrupt(digitalPinToInterrupt(SW_PIN), readButton, FALLING);   // Attach interrupt to SW pin
}

void loop() {
  // Print the encoder count and direction
  Serial.print("Counter: ");
  Serial.print(encoder_counter);
  Serial.print(" Direction: ");
  Serial.println(direction == 1 ? "Clockwise" : "Counter-clockwise");

  // Check if the button was pressed
  if (buttonPressed) {
    Serial.println("Button Pressed!");
    buttonPressed = false;  // Reset button press state
  }

  delay(100);  // Small delay to avoid flooding the serial monitor
}

void readEncoder() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce the encoder signal
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(CLK_PIN) == digitalRead(DT_PIN)) {
      encoder_counter++;
      direction = 1;  // Clockwise
    } else {
      encoder_counter--;
      direction = -1;  // Counter-clockwise
    }
  }
  lastInterruptTime = interruptTime;
}

void readButton() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce the button press
  if (interruptTime - lastInterruptTime > 50) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}
