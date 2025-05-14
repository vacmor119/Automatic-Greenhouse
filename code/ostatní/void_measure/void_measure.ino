void measure() {
  temperatureMeasured = dht.readTemperature();    //zsikani teploty z DHT11
  humidityMeasured = dht.readHumidity();          //ziskani vlhkosti z DHT11
  soilMoistureMeasured = analogRead(SoilMoistureSensor);    //mereni vlhkosti pudy
  Serial.print("Teplota z DHT11:  ");     //test
  Serial.print(temperatureMeasured);
  Serial.print(" a vlhkost:  ");
  Serial.print(humidityMeasured);
  Serial.print(" a vlhkost půdy:  ");
  Serial.prinln(soilMoistureMeasured);
}
