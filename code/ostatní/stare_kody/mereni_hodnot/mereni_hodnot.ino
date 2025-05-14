#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define Relay 3

float humidity;
float temperature;

void setup() {

  Serial.begin(9600);
  dht.begin();
  pinMode(DHTPIN,INPUT);
  pinMode(Relay,OUTPUT);

}

void loop() {
  
  // Read humidity
  humidity = dht.readHumidity();
  
  // Read temperature in Celsius
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.println(humidity);
  Serial.println(temperature);
  if(temperature<25){
    digitalWrite(Relay,HIGH);
    Serial.println("Relay ON!");
  }
  else {
    digitalWrite(Relay,LOW);
    Serial.println("Relay OFF!");
  } 
  delay(10000);
}
