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

int i;

void setup()
{
  lcd.init();  // initialize the lcd 
  lcd.backlight();

  lcd.createChar(0, degree);
  lcd.createChar(1, pointer);
}


void loop()
{
  LCD_menu();

  for (i=0;i<4;i++) {
  lcd.setCursor(0,i);
  lcd.printByte(1);
  delay(1000);
  lcd.setCursor(0,i);
  lcd.print(" ");
  }

}

void LCD_menu()
{
  lcd.setCursor(1,0); //menu
  lcd.print("temperature:      C");
  lcd.setCursor(1,1);
  lcd.print("humidity:     %");
  lcd.setCursor(1,2);
  lcd.print("soil moisture:  %");
  lcd.setCursor(1,3);
  lcd.print("Grow LED's:");
  lcd.setCursor(18,0);
  lcd.printByte(0);
}
