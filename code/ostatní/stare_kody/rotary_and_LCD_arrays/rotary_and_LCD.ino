//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t degree[8]  = {0x07, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t pointer[8]  = {0x10, 0x18, 0x1c, 0x1e, 0x1e, 0x1c, 0x18, 0x10};

// Define pins
#define clkPin 4 // CLK pin
#define dtPin 3  // DT pin
#define swPin 2  // SW pin

// Global variable to store encoder data
volatile long menuValue = 0;

// Previous states of pins
volatile int lastCLKState = 0;

volatile int setValues[4] = {20,80,60,50};

volatile bool settingValue = true;

int lastCursorPosition = 0;


void setup()
{
  Serial.begin(9600);

  lcd.init();  // initialize the lcd 
  lcd.backlight();

  lcd.createChar(0, degree);
  lcd.createChar(1, pointer);

  LCD_menu();

  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(dtPin), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(swPin), readButton, FALLING);
}


void loop()
{
  cursor();
  LCD_data();
}

void LCD_menu()
{
  lcd.setCursor(1,0); //menu
  lcd.print("temperature:      C");
  lcd.setCursor(1,1);
  lcd.print("humidity:         %");
  lcd.setCursor(1,2);
  lcd.print("soil moisture:    %");
  lcd.setCursor(1,3);
  lcd.print("Grow LED's:       %");
  lcd.setCursor(18,0);
  lcd.printByte(0);
}

void LCD_data()
{
  lcd.setCursor(16,0); //data
  lcd.print(setValues[0]);
  lcd.setCursor(16,1);
  lcd.print(setValues[1]);
  lcd.setCursor(16,2);
  lcd.print(setValues[2]);
  lcd.setCursor(16,3);
  lcd.print(setValues[3]);
}

// Encoder interrupt service routine for CLK
void readEncoder() {
  int clkState = digitalRead(clkPin);
  int dtState = digitalRead(dtPin);
  if(!settingValue) {
    if (clkState != lastCLKState && clkState == LOW) {
      if (dtState != clkState) {
        menuValue++;
        if(menuValue > 4) {
          menuValue = 0;
        }
      } else {
        menuValue--;
        if (menuValue < 0) {
          menuValue = 4;
        }
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

void cursor()
{
  if(lastCursorPosition != menuValue){
    lcd.setCursor(0,lastCursorPosition);
    lcd.print(" ");
  }
  lastCursorPosition = menuValue;
  lcd.setCursor(0,menuValue);
  lcd.write(1);
}