//pridani nastaveni casu manualne pomoci enkoderu?

#include <Wire.h> //I2C
#include <LiquidCrystal_I2C.h>  //od YWROBOT
#include "DHT.h"    // knihovna na mereni z DHT11
#include <EEPROM.h>   // pro zapis do interni EEPROM
#include <virtuabotixRTC.h> //RTC modul DS1302

LiquidCrystal_I2C lcd(0x27,20,4);   // LCD2004, 0x27 adresa v hex, 20 - pocet sloupcu, 4 pocet radku

#if defined(ARDUINO) && ARDUINO >= 100      // čast pro knihovnu na tvoreni custom znaku
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

//DS1302 setup
virtuabotixRTC RTC(15, 16, 17); //A1-A3

uint8_t degree[8]  = {0x07, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00};                    // custom znak
uint8_t pointerSettingValue[8]  = {0x10, 0x18, 0x1c, 0x1e, 0x1e, 0x1c, 0x18, 0x10};      // custom znak
uint8_t pointerNotSettingValue[8]  = {0x10, 0x18, 0x14, 0x12, 0x12, 0x14, 0x18, 0x10};   // custom znak 
uint8_t z_s_hackem[8] = { 0x0A, 0x04, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00};
uint8_t i_s_hackem[8] = { 0x02, 0x04, 0x00, 0x0C, 0x04, 0x04, 0x0E, 0x00 };
uint8_t e_s_hackem[8] = { 0x0A, 0x04, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 };

#define soilMoistureSensor A0
// Rotary encoder
#define swPin 2   // SW pin
#define dtPin 3   // DT pin
#define clkPin 4  // CLK pin
//relay board setup
#define airHumidifier 5    // air humidifier
#define relay1 6    // topny teleso
#define relay2 7    // Grow LED's
#define relay3 8    // Čerpadlo
//DHT 11 setup
#define dhtPin 9    // DHT11 na pinu 9
#define fan 10    //PWM na vetraky
#define timeAdjustButton 13   //tlacitko na serizeni casu

#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);

bool casZalevani;       //zda se muze zalevat
bool casSviceni;       // Zda se ma svitit

bool timeAdjust = false; //serizeni casu ano/ne

// Global variable to store encoder data
volatile int menuValue = 0;           //menuValue = 0 - temperature, menuValue = 1 - humidity, menuValue = 2 - soil moisture, menuValue = 3 - Grow LED's
volatile bool settingValue = true;    //tohle toggluje ten button mezi T/F

volatile int lastCLKState = 0;      // Previous states of pins

volatile float temperatureSet = 20;   // Promenna na nastavenou hodnotu
volatile float humiditySet = 20;      // Promenna na nastavenou hodnotu
volatile float soilMoistureSet = 20;  // Promenna na nastavenou hodnotu
volatile bool growLedSet = true;     // Promenna na nastavenou hodnotu

float temperatureMeasured;        // Promenna na zmerenou hodnotu
float humidityMeasured;           // Promenna na zmerenou hodnotu
float soilMoistureMeasured = 60;  // Promenna na zmerenou hodnotu - tady je 60, protoze senzor na vlhkost pudy jsem jeste nezapojil

int lastCursorPosition = 0;     // aby vedel, kde byl cursor naposled

unsigned long previousMillis = 0; // Stores the last time the measure, time and eeprom was performed
const long interval = 5000; // Interval at which to perform the measure, time and eeprom
unsigned long currentMillis;

const int debounceDelay = 500; // Debounce delay in milliseconds
volatile unsigned long lastDebounceTime = 0; // Last time the button was pressed

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
  //EEPROM.put(temperatureSetAddress, temperatureSet);       //jen kdyz se nahrava na arduino, kde neni nic v EEPROMCE
  //EEPROM.put(humiditySetAddress, humiditySet);
  //EEPROM.put(soilMoistureSetAddress, soilMoistureSet);
  //EEPROM.put(growLedSetAddress, growLedSet);

  // Initialize global variables with values from EEPROM
  EEPROM.get(temperatureSetAddress, temperatureSet);
  EEPROM.get(humiditySetAddress, humiditySet);
  EEPROM.get(soilMoistureSetAddress, soilMoistureSet);
  EEPROM.get(growLedSetAddress, growLedSet);

  Serial.begin(9600);

  lcd.init();                 // initializace lcd 
  lcd.backlight();            // zapnuti podsviceni

  lcd.createChar(0, degree);                      // vytvoreni znaku stupne
  lcd.createChar(1, pointerSettingValue);        // vytvoreni znaku plne sipky
  lcd.createChar(2, pointerNotSettingValue);     // vytvoreni znaku prazdne sipky
  lcd.createChar(3, z_s_hackem);                  //vytvoreni znaku ž
  lcd.createChar(4, i_s_hackem);                  //vytvoreni znaku í
  lcd.createChar(5, e_s_hackem);                  //vytvoreni znaku ě


  dht.begin();                  //start DHT11
  pinMode(dhtPin,INPUT);

  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 25, 8, 6, 3, 5, 2025);

  LCD_menu();       // hned ze zacatku zobrazi menu

  pinMode(clkPin, INPUT);         //nastaveni pinů
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  pinMode(timeAdjustButton,INPUT_PULLUP);

  pinMode(airHumidifier,OUTPUT);
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(dtPin), readEncoder, CHANGE);       //definovani interruptů
  attachInterrupt(digitalPinToInterrupt(swPin), readButton, FALLING);
}


void loop(){
  if (settingValue) {
    constrainValues();    //aby uzivatel nenastavil nejakou hloupost - napr. vlhkost vzduchu 800%
    LCD_set();            //zobrazi na LCD nastavene hodnoty
  } else {
    LCD_measure();        //zobrazi na LCD zmerene hodnoty
  }
  cursor();               //aktualizovani cursoru

  currentMillis = millis();                             //millis() - kazdych 5s se zmeri hodnoty, zjisti cas a probehne zapis nastavenych hodnot do EEPROM (pokud se zmenily)
  if (currentMillis - previousMillis >= interval) {         
    previousMillis = currentMillis;                         
    measure();
    saveToEEPROM();
    time();
  }
  
  //TAHLE CELA CAST JE JENOM DEMO KDE JSEM TESTOVAL, ZDA ZATIM VSE FUNGUJE JAK MA (ve finale tam budou nejspis podminky s millis())
  if (humidityMeasured < humiditySet) {     
    digitalWrite(airHumidifier,HIGH);            
  } else {
  }
  if (temperatureMeasured < temperatureSet) {
    digitalWrite(relay1,HIGH);            
  }
  if (growLedSet && casSviceni) {
    digitalWrite(relay2,HIGH);
  }
  if(!growLedSet || !casSviceni) {
    digitalWrite(relay2,LOW);
  }
  if (soilMoistureMeasured < soilMoistureSet && casZalevani) {
    digitalWrite(relay3,HIGH);            
  }
}

//zobrazi zakladni text na lcd
void LCD_menu() {
  lcd.setCursor(1,0);               //menu
  lcd.print("teplota:          C");
  lcd.setCursor(1,1);
  lcd.print("vlhkost:         %");
  lcd.setCursor(1,2);
  lcd.print("zavla en :       %");
  lcd.setCursor(6,2);
  lcd.printByte(3);
  lcd.setCursor(9,2);
  lcd.printByte(4);
  lcd.setCursor(1,3);
  lcd.print("osv tlen :");
  lcd.setCursor(9,3);
  lcd.printByte(4);
  lcd.setCursor(4,3);
  lcd.printByte(5);
  lcd.setCursor(18,0);
  lcd.printByte(0);
}

//zobrazi nastavene hodnoty
void LCD_set() {
  lcd.setCursor(13,0);              
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

//zobrazi zmerene hodnoty
void LCD_measure() {
  lcd.setCursor(13,0);               
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

// Encoder interrupt service routine
void readEncoder() {
  detachInterrupt(digitalPinToInterrupt(dtPin));
  int clkState = digitalRead(clkPin);       //precteni a ulozeni urovne clk pinu
  int dtState = digitalRead(dtPin);         //precteni a ulozeni urovne dt pinu
  if(!settingValue) {                       //kdyz se nenastavuje hodnota, tak se jen posouva sipkou mezi 4 radky a vybira se parametr na nastavovani
    if (clkState != lastCLKState && clkState == LOW) {    
      if (dtState != clkState) {
        menuValue++;
        if(menuValue > 3) {       // menuValue = 0 - temperature
          menuValue = 0;          // menuValue = 1 - humidity
        }                         // menuValue = 2 - soil moisture
      } else {                    // menuValue = 3 - Grow LED's
        menuValue--;
        if (menuValue < 0) {      //lcd ma jenom 4 radky
          menuValue = 3;
        }
      }
    }
  }
  if(settingValue) {                                       //kdyz nastavujeme hodnotu parametru, otaceni po smeru hod. rucicek +0,5 ; otaceni proti smeru hod. rucicek -0,5
    if (clkState != lastCLKState && clkState == LOW) {
      if (dtState != clkState) {
        switch (menuValue+1) {    //menuValue+1 protoze nejde case 0
          case 1:
            temperatureSet = temperatureSet + 0.5;    //v podstate jen meni nastavenou hodnotu o 0,5
            break;
          case 2:
            humiditySet = humiditySet + 1;
            break;
          case 3:
            soilMoistureSet = soilMoistureSet + 1;
            break;
          case 4:
            growLedSet = true;
            break;
        }
      } else {
        switch (menuValue+1) {
          case 1:
            temperatureSet = temperatureSet - 0.5;
            break;
          case 2:
            humiditySet = humiditySet - 1;
            break;
          case 3:
            soilMoistureSet = soilMoistureSet - 1;
            break;
          case 4:
            growLedSet = false;
            break;
        }
      }
    }
  }
  lastCLKState = clkState;    //ulozeni posledni urovne clock
  attachInterrupt(digitalPinToInterrupt(dtPin), readEncoder, CHANGE);
}

// Button interrupt service routine (tlacitko v encoderu)
void readButton() {
  currentMillis = millis();                                              // debounce tlacitka (to tlacitko v encoderu je dost pomaly)
  if (currentMillis - lastDebounceTime > debounceDelay) {
    detachInterrupt(digitalPinToInterrupt(swPin));                       // vypnuti interruptu dokud neprobehne cela funkce
    Serial.print("Rotary encoder button pressed! ");
    settingValue = !settingValue;                                        //prepinani mezi nastavovanim hodnoty a vybiranim parametru
    Serial.print("settingValue je: ");
    Serial.println(settingValue);
    lastDebounceTime = currentMillis;                                    // promenna na porovnavani s millis()
  }
  attachInterrupt(digitalPinToInterrupt(swPin), readButton, FALLING);    //zase aktivovat interrupt
}

void cursor(){
  if(lastCursorPosition != menuValue){      //vymazani predchoziho cursoru
    lcd.setCursor(0,lastCursorPosition);
    lcd.print(" ");
  }
  lastCursorPosition = menuValue;
  lcd.setCursor(0,menuValue);               //napsani cursoru, zalezi zda bude "duta" sipka nebo "plna" sipka podle toho zda nastavujeme hodnotu nebo vybirame parametr
  if(settingValue){
    lcd.write(1);
  } else {
  lcd.write(2);
  }
}

void measure(){
  temperatureMeasured = dht.readTemperature();    //zsikani teploty z DHT11
  humidityMeasured = dht.readHumidity();          //ziskani vlhkosti z DHT11
  soilMoistureMeasured = map(analogRead(soilMoistureSensor),0,1023,0,100); //analog signal to %
  //jeste mereni vlhkosti pudy
  Serial.print("Teplota z DHT11:  ");     //test
  Serial.print(temperatureMeasured);
  Serial.print(" a vlhkost:  ");
  Serial.println(humidityMeasured);
}

// ukladani promennych do EEPROM
void saveToEEPROM() {
  float eepromTemperatureSet;
  float eepromHumiditySet;
  float eepromSoilMoistureSet;
  bool eepromGrowLedSet;

  // precteni ulozenych hodnot v EEPROM
  EEPROM.get(temperatureSetAddress, eepromTemperatureSet);  // .get() zvlada narozdil od .read() i float a bool atd.
  EEPROM.get(humiditySetAddress, eepromHumiditySet);
  EEPROM.get(soilMoistureSetAddress, eepromSoilMoistureSet);
  EEPROM.get(growLedSetAddress, eepromGrowLedSet);

  // ulozi do EEPROM hodnoty jen kdyz se zmeni, jinak by se to porad prepisovalo a EEPROMka ma omezeny pocet zapisu
  if (eepromTemperatureSet != temperatureSet) {
    EEPROM.put(temperatureSetAddress, temperatureSet);          // .put() zvlada narozdil od .write() i float a bool atd.
    Serial.println("Probehlo prepsani temp do EEPROM!") ;       //zas jenom test jestli se prepisuji spravne veci
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

//RTC modul DS1302
void time() {
  RTC.updateTime();             //nacteni casu z RTC
  int hour = RTC.hours;         //ulozeni hoidny

  if ((hour >= 6 && hour <= 9) || (hour >= 15 && hour <= 17)) {       //&& ma prednost pred ||, proto to musi byt v navic zavorkach
    casZalevani = true;                                               // napr neni dobre zalevat rostliny v noci/poledne
    Serial.println("casZalevani je true!");
  } else {
    casZalevani = false;                
    Serial.println("casZalevani je false!");
  }

  if(hour >= 6 && hour <= 19) {
    casSviceni = true;                                        //zapnuti sviceni
    lcd.backlight();                                          //zapnuti podsviceni lcd
    Serial.println("casSviceni je true!");
  } else {
    casSviceni = false;                                       //at to v noci nikoho neprudí svícením nebo tak neco a rostliny potrebujou taky tmu
    lcd.noBacklight();                                        //vypnuti podsviceni lcd
    Serial.println("casSviceni je false!");
  }
}










