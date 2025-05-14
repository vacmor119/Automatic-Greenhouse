//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"    // knihovna na mereni z DHT11
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,20,4);   // LCD2004, 0x27 adresa v hex, 20 - pocet sloupcu, 4 pocet radku

#if defined(ARDUINO) && ARDUINO >= 100      // čast pro knihovnu na tvoreni custom znaku
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t degree[8]  = {0x07, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00};                    // custom znak
uint8_t pointer_settingValue[8]  = {0x10, 0x18, 0x1c, 0x1e, 0x1e, 0x1c, 0x18, 0x10};      // custom znak
uint8_t pointer_notSettingValue[8]  = {0x10, 0x18, 0x14, 0x12, 0x12, 0x14, 0x18, 0x10};   // custom znak

// Rotary encoder
#define clkPin 4 // CLK pin
#define dtPin 3  // DT pin
#define swPin 2  // SW pin

const int debounceDelay = 200; // Debounce delay in milliseconds
volatile unsigned long lastDebounceTime = 0; // Last time the button was pressed

//DHT 11 setup
#define DHTPIN 9          // DHT11 na pinu 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//relay board setup
#define relay1 5    // air humidifier
#define relay2 6    // topny teleso
#define relay3 7    // Grow LED's
#define relay4 8    // Čerpadlo

// Global variable to store encoder data
volatile int menuValue = 0;           //menuValue = 0 - temperature, menuValue = 1 - humidity, menuValue = 2 - soil moisture, menuValue = 3 - Grow LED's
volatile bool settingValue = true;    //tohle toggluje ten button mezi T/F

volatile int lastCLKState = 0;      // Previous states of pins

volatile float temperatureSet;   // Promenna na nastavenou hodnotu
volatile float humiditySet;      // Promenna na nastavenou hodnotu
volatile float soilMoistureSet;  // Promenna na nastavenou hodnotu
volatile bool growLedSet;     // Promenna na nastavenou hodnotu

float temperatureMeasured;        // Promenna na zmerenou hodnotu
float humidityMeasured;           // Promenna na zmerenou hodnotu
float soilMoistureMeasured = 60;  // Promenna na zmerenou hodnotu

int lastCursorPosition = 0;     // aby vedel, kde byl cursor naposled

unsigned long previousMillis = 0; // Stores the last time the action was performed
const long interval = 5000; // Interval at which to perform the action (milliseconds)
unsigned long currentMillis;

const int temperatureSetAddress = 0;    //adresa v EEPROM
const int humiditySetAddress = 4;       //adresa v EEPROM
const int soilMoistureSetAddress = 8;   //adresa v EEPROM
const int growLedSetAddress = 12;       //adresa v EEPROM

// Function to set the value within a specific range using constrain()
void constrainValues() {
  temperatureSet = constrain(temperatureSet, 15, 40); // Constrain value between 15 and 40
  humiditySet = constrain(humiditySet, 15, 80); // Constrain value between 15 and 80
  soilMoistureSet = constrain(soilMoistureSet, 5, 75); // Constrain value between 5 and 75
}

void setup(){
  // Initialize global variables with values from EEPROM
  EEPROM.get(temperatureSetAddress, temperatureSet);
  EEPROM.get(humiditySetAddress, humiditySet);
  EEPROM.get(soilMoistureSetAddress, soilMoistureSet);
  EEPROM.get(growLedSetAddress, growLedSet);

  Serial.begin(9600);

  lcd.init();                 // initialize the lcd 
  lcd.backlight();            // zapnuti podsviceni

  lcd.createChar(0, degree);                      // vytvoreni znaku stupne
  lcd.createChar(1, pointer_settingValue);        // vytvoreni znaku plne sipky
  lcd.createChar(2, pointer_notSettingValue);     // vytvoreni znaku prazdne sipky

  dht.begin();
  pinMode(DHTPIN,INPUT);

  LCD_menu();       // hned ze zacatku zobrazi menu

  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(dtPin), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(swPin), readButton, FALLING);
}


void loop(){
  if (settingValue) {
    constrainValues();
    LCD_set();
  } else {
    LCD_measure();
  }
  cursor();

  currentMillis = millis();
  // Check if interval has passed
  if (currentMillis - previousMillis >= interval) {         // Save the last time the action was performed
    previousMillis = currentMillis;                         // Perform the action (e.g., toggle an LED)
    measure();
    saveToEEPROM();
    Serial.println("Saving to EEPROM!");
  }
  if (temperatureMeasured < temperatureSet) {
    digitalWrite(relay2,HIGH);
  } else {
    digitalWrite(relay2,LOW);
  }
  if (humidityMeasured < humiditySet) {
    digitalWrite(relay1,HIGH);
  } else {
    digitalWrite(relay1,LOW);
  }
  if (soilMoistureMeasured < soilMoistureSet) {
    digitalWrite(relay4,HIGH);
  } else {
    digitalWrite(relay4,LOW);
  }
}

void LCD_menu() {
  lcd.setCursor(1,0); //menu
  lcd.print("temperature:      C");
  lcd.setCursor(1,1);
  lcd.print("humidity:         %");
  lcd.setCursor(1,2);
  lcd.print("soil moist:       %");
  lcd.setCursor(1,3);
  lcd.print("Grow LED's:");
  lcd.setCursor(18,0);
  lcd.printByte(0);
}

void LCD_set() {
  lcd.setCursor(13,0);              //display set values
  lcd.print(temperatureSet);
  lcd.setCursor(13,1);
  lcd.print(humiditySet);
  lcd.setCursor(13,2);
  lcd.print(soilMoistureSet);
  lcd.setCursor(13,3);
  if (growLedSet) {
  lcd.print("ON ");
  } else {
  lcd.print("OFF");
  }
}

void LCD_measure() {
  lcd.setCursor(13,0);               //display measured values
  lcd.print(temperatureMeasured);
  lcd.setCursor(13,1);
  lcd.print(humidityMeasured);
  lcd.setCursor(13,2);
  lcd.print(soilMoistureMeasured);
  lcd.setCursor(13,3);
  if (growLedSet) {
  lcd.print("ON ");
  } else {
  lcd.print("OFF");
  }
}

// Encoder interrupt service routine for CLK
void readEncoder() {
  int clkState = digitalRead(clkPin);
  int dtState = digitalRead(dtPin);
  if(!settingValue) {
    if (clkState != lastCLKState && clkState == LOW) {
      if (dtState != clkState) {
        menuValue++;
        if(menuValue > 3) {       // menuValue = 0 - temperature
          menuValue = 0;          // menuValue = 1 - humidity
        }                         // menuValue = 2 - soil moisture
      } else {                    // menuValue = 3 - Grow LED's
        menuValue--;
        if (menuValue < 0) {
          menuValue = 3;
        }
      }
    }
  }
  if(settingValue) {
    if (clkState != lastCLKState && clkState == LOW) {
      if (dtState != clkState) {
        switch (menuValue+1) {
          case 1:
            temperatureSet = temperatureSet + 0.5;
            break;
          case 2:
            humiditySet = humiditySet + 0.5;
            break;
          case 3:
            soilMoistureSet = soilMoistureSet + 0.5;
            break;
          case 4:
            growLedSet = true;
            digitalWrite(relay3,HIGH);
            break;
        }
      } else {
        switch (menuValue+1) {
          case 1:
            temperatureSet = temperatureSet - 0.5;
            break;
          case 2:
            humiditySet = humiditySet - 0.5;
            break;
          case 3:
            soilMoistureSet = soilMoistureSet - 0.5;
            break;
          case 4:
            growLedSet = false;
            digitalWrite(relay3,LOW);
            break;
        }
      }
    }
  }
  lastCLKState = clkState;
}

// Button interrupt service routine for SW
void readButton() {
  unsigned long currentTime = millis(); // Check if enough time has passed since the last button press
  if (currentTime - lastDebounceTime > debounceDelay) {
    detachInterrupt(digitalPinToInterrupt(swPin)); // Disable interrupt to avoid multiple triggers
    Serial.print("Rotary encoder button pressed! ");
    settingValue = !settingValue;
    Serial.print("settingValue je: ");
    Serial.println(settingValue);
    lastDebounceTime = currentTime; // Update the last debounce time
  }
  // Re-enable the interrupt
  attachInterrupt(digitalPinToInterrupt(swPin), readButton, FALLING);
}

void cursor(){
  if(lastCursorPosition != menuValue){
    lcd.setCursor(0,lastCursorPosition);
    lcd.print(" ");
  }
  lastCursorPosition = menuValue;
  lcd.setCursor(0,menuValue);
  if(settingValue){
    lcd.write(1);
  } else {
  lcd.write(2);
  }
}

void measure(){
  temperatureMeasured = dht.readTemperature();
  humidityMeasured = dht.readHumidity();
  //jeste mereni vlhkosti pudy
  Serial.print("Teplota z DHT11:  ");
  Serial.print(temperatureMeasured);
  Serial.print(" a vlhkost:  ");
  Serial.println(humidityMeasured);
}

// Function to save variables to EEPROM
void saveToEEPROM() {
  float eepromTemperatureSet;
  float eepromHumiditySet;
  float eepromSoilMoistureSet;
  bool eepromGrowLedSet;

  // Read the current values from EEPROM
  EEPROM.get(temperatureSetAddress, eepromTemperatureSet);  // .get() zvlada narozdil od .read() i float a bool atd.
  EEPROM.get(humiditySetAddress, eepromHumiditySet);
  EEPROM.get(soilMoistureSetAddress, eepromSoilMoistureSet);
  EEPROM.get(growLedSetAddress, eepromGrowLedSet);

  // Compare and save only if the values are different
  if (eepromTemperatureSet != temperatureSet) {
    EEPROM.put(temperatureSetAddress, temperatureSet);  // .put() zvlada narozdil od .write() i float a bool atd.
    Serial.println("Probehlo prepsani temp do EEPROM!") ;
    }
  if (eepromHumiditySet != humiditySet) {
    EEPROM.put(humiditySetAddress, humiditySet);
    Serial.println("Probehlo prepsani hum do EEPROM!") ;
  }
  if (eepromSoilMoistureSet != soilMoistureSet) {
    EEPROM.put(soilMoistureSetAddress, soilMoistureSet);
    Serial.println("Probehlo prepsani soilm do EEPROM!") ;
  }
  if (eepromGrowLedSet != growLedSet) {
    EEPROM.put(growLedSetAddress, growLedSet);
    Serial.println("Probehlo prepsani growLED do EEPROM!") ;
  }
}










