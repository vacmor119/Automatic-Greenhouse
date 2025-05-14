#include <virtuabotixRTC.h>                                                                              

virtuabotixRTC myRTC(15, 16, 17);

void setup()  {      
  Serial.begin(9600);

// Set the current date, and time in the following format:
// seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 25, 8, 6, 3, 5, 2025);
}

void loop()  {                                                                                           
// This allows for the update of variables for time or accessing the individual elements.                
  myRTC.updateTime();                                                                                    
                                                                                                         
// Start printing elements as individuals                                                                
  Serial.print("Current Date / Time: ");                                                                 
  Serial.print(myRTC.dayofmonth);                                                                        
  Serial.print("/");                                                                                     
  Serial.print(myRTC.month);                                                                             
  Serial.print("/");                                                                                     
  Serial.print(myRTC.year);                                                                              
  Serial.print("  ");                                                                                    
  Serial.print(myRTC.hours);                                                                             
  Serial.print(":");                                                                                     
  Serial.print(myRTC.minutes);                                                                           
  Serial.print(":");                                                                                     
  Serial.println(myRTC.seconds);                                                                        
                                                                                                        
  delay(1000);                                                                                          
}                                                                                                       
