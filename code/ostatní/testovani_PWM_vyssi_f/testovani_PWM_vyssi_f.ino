#define fan 9    // vetraky pro vetrani

void setup() {
  pinMode(fan,OUTPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 0x01; // Nastavení prescaleru na 1 (maximální frekvence)
}

void loop() {
  for(int i = 0; i < 256; i++){
    analogWrite(fan,i);
    delay(50);
  }
  delay(3000);
  for(int i = 255; i > 0; i--){
    analogWrite(fan,i);
    delay(50);
  }
  delay(3000);
}
