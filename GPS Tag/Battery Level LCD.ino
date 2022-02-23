#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

float input_voltage = 0.0;
float temp=0.0;
float volt_percent = 0.0;
float r1=4000.0;
float r2=10000.0;


void setup()
{
   lcd.init();             // initialize the lcd 
   lcd.backlight();
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps
   lcd.begin(16, 2);       //// set up the LCD's number of columns and rows: 
   lcd.print("BATTERY STATUS");
   delay(300);
}
void loop()
{
   
//Conversion formula

   int analog_value = analogRead(A0);
    temp = (analog_value * 5.0) / 1024.0; 
   input_voltage = temp / (r2/(r1+r2));
   volt_percent = (input_voltage/7.0)*100;
   
   
   if (volt_percent == 100)
      {
     Serial.print("v= ");
     Serial.println(input_voltage);
     lcd.setCursor(0, 1);
     lcd.print("Full Battery");
     lcd.print(volt_percent);
     lcd.print("%");
     delay(1000);
     lcd.clear();
   }
   else if (volt_percent < 100)
   {
     Serial.print("v= ");
     Serial.println(input_voltage);
     lcd.setCursor(0, 0);
     lcd.print("HighBatteryLevel");
     lcd.setCursor(0, 1);
     lcd.print(volt_percent);
     lcd.print("%");
     delay(1000);
     lcd.clear();
   }
   else if (volt_percent < 50)
   {
     lcd.setCursor(0, 0);
     lcd.print("Middle Battery Level");
     lcd.setCursor(0, 1);
     lcd.print(volt_percent);
     lcd.print("%");
     delay(1000);
     lcd.clear();
   }
    else if (volt_percent < 10)
   {
     lcd.setCursor(0, 0);
     lcd.print("Low Battery Level");
     lcd.setCursor(0, 1);
     lcd.print(volt_percent);
     lcd.print("%");
     delay(1000);
     lcd.clear();
   }
   
}
