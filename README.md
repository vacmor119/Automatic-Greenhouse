# Automatický Skleník
## ⚠️WIP⚠️
### Features
This is a simple `README.md` structure to demonstrate formatting.

* **Bold** and *Italic* text
- Bullet points for clarity
- Structured headings
- An image for visual appeal
### Kód
- jednotlivé **funkce** a **globální** proměnné pro **přehlednost**
- kousek kódu:

```
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

```
### Vizualizace
- předběžná představa realizace:

<p align="center">
<img src="https://github.com/vacmor119/Automatic-Greenhouse/blob/main/images/vizualizace/pohled1.png" alt="Example Image" width="700"/>
</p>

🔗 [Visit the Project on GitHub](https://github.com/vacmor119/Automatic-Greenhouse)

