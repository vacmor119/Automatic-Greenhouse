# Automatic Greenhouse
## Overview
### Features
This is a simple `README.md` structure to demonstrate formatting.

* **Bold** and *Italic* text
- Bullet points for clarity
- Structured headings
- An image for visual appeal

'//RTC modul DS1302
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
}'


![Example Image](https://github.com/vacmor119/Automatic-Greenhouse/blob/main/images/vizualizace/pohled1.png)

<p align="center">
<img src="https://github.com/vacmor119/Automatic-Greenhouse/blob/main/images/vizualizace/pohled1.png" alt="Example Image" width="400"/>
</p>

🔗 [Visit the Project on GitHub](https://github.com/vacmor119/Automatic-Greenhouse)

Feel free to modify and expand upon it to suit your project's needs! 🚀
