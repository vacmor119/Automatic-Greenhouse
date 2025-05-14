#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
bool debugging = "False";



void setup() {
  Serial.begin(9600);
  dht.begin();
}



void set(float* humidity, float* temperature) {
  *humidity = analogRead(A0) / 10.24;
  *temperature = analogRead(A1) / 25.6;
}



void measure(float* humidity, float* temperature){ //tady bude jeste potreba pridat vlhkost pudy
  delay(2000);

  // Read humidity
  *humidity = dht.readHumidity();
  
  // Read temperature in Celsius
  *temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(*humidity) || isnan(*temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}



void loop() {
  float humidity_set, temperature_set;
  set(&humidity_set, &temperature_set); // Now you can use humidity_set and temperature_set in the loop()
   

  float humidity_measured, temperature_measured;
  measure(&humidity_measured, &temperature_measured); // zmeri teplotu a vlhkost vzduchu a pudy

  //compare();
  if (debugging){
  debug(humidity_set,temperature_set,humidity_measured,temperature_measured);
  }
  
  delay(5000);
}



void compare(){
  if (temperature_set < temperature_measured){
    //otevre se nejake okenko pomoci serva asi
  }
  else{
    //zavre se okenko
  }

  if humidity_set > humidity_measured{
    //zapne se nejaky zvlhcovac (hadr a vetracek??? nebo nejaky postrikovac co bude mackat servo???)
  }
  //tady bude jeste vlhkost pudy a pumpa
}



void debug(int humidity_set, int temperature_set, int humidity_measured, int temperature_measured){
  String humidityStringSet = String(humidity_set);
  String temperatureStringSet = String(temperature_set);
  Serial.print("Humidity is set to: " + humidityStringSet + " % \t");
  Serial.println("Temperature is set to: " + temperatureStringSet + " *C");

  String humidityStringMeasured = String(humidity_measured);
  String temperatureStringMeasured = String(temperature_measured);
  Serial.print("Humidity measured: " + humidityStringMeasured + " % \t");
  Serial.println("Temperature measured: " + temperatureStringMeasured + " *C"); 
}
