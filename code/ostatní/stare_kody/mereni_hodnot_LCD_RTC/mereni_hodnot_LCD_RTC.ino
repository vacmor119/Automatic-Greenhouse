#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

DS3231 clock;
RTCDateTime dt;

float humidity;
float temperature;
float humidityLCD;
float temperatureLCD;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {

  Serial.begin(9600);

  dht.begin();
  pinMode(DHTPIN,INPUT);

  clock.begin();
  clock.setDateTime(__DATE__, __TIME__);

  lcd.init();  // initialize the lcd 
  lcd.backlight();

  lcd.setCursor(1,0); //menu
  lcd.print("temperature:     oC");
  lcd.setCursor(1,1);
  lcd.print("humidity:     %");
  lcd.setCursor(1,2);
  lcd.print("soil moisture:  %");
  lcd.setCursor(1,3);
  lcd.print("Grow LED's:");
}

void loop() {
  
  dt = clock.getDateTime();

  time();
  // Read humidity
  humidity = dht.readHumidity();
  
  // Read temperature in Celsius
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if(temperatureLCD!=temperature)
  {
    temperatureLCD = temperature;
    LCD();
    Serial.println("Probehl prepis teploty!");
  }
  if(humidityLCD!=humidity)
  {
    humidityLCD = humidity;
    LCD();
    Serial.println("Probehl prepis vlhkosti!");
  }
  delay(10000);
}

void LCD()
{
  lcd.setCursor(13,0);
  lcd.print(temperatureLCD);
  lcd.setCursor(10,1);
  lcd.print(humidityLCD);
}

void time()
{
  Serial.print(dt.hour);
  Serial.print(":");
  Serial.print(dt.minute);
  Serial.print(":");
  Serial.print(dt.second);
  Serial.print("      ");
  Serial.print(dt.day);
  Serial.print("/");
  Serial.print(dt.month);
  Serial.print("/");
  Serial.println(dt.year);
}
