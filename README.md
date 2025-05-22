# Automatický Skleník
## ⚠️WIP⚠️
### Features
### Kód
- jednotlivé **funkce** a **globální** proměnné pro **přehlednost**
- celá hotová část kódu `základ_kódu.ino` je ve složce *code*
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

