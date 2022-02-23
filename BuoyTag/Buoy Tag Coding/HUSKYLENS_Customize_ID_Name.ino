#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

HUSKYLENS huskylens;

SoftwareSerial mySerial(11, 12);

void setup() 
{
    Serial.begin(115200);
    mySerial.begin(9600);
    while (!huskylens.begin(mySerial))
    {
        Serial.println(F("Begin failed!"));
        delay(100);
    }
    while (!huskylens.setCustomName("PlasticBottle",1))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID1 customname failed!")); 
      delay(100);
    }
    while (!huskylens.setCustomName("Non",2))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID2 customname failed!")); 
      delay(100);
    }
    while (!huskylens.setCustomName("Can",3))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID3 customname failed!")); 
      delay(100);
    }
     while (!huskylens.setCustomName("Bottle",4))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID4 customname failed!")); 
      delay(100);
    }
    while (!huskylens.setCustomName("Can",5))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID5 customname failed!")); 
      delay(100);
    }
     while (!huskylens.setCustomName("Bottle",6))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID6 customname failed!")); 
      delay(100);
    }
     while (!huskylens.setCustomName("Plastic",7))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID7 customname failed!")); 
      delay(100);
    }
     while (!huskylens.setCustomName("Plastic",8))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID8 customname failed!")); 
      delay(100);
    }
     while (!huskylens.setCustomName("Plastic",9))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID9 customname failed!")); 
      delay(100);
    }
     while (!huskylens.setCustomName("Plastic",10))  // bool setCustomName(String name,uint8_t id)
    {
      Serial.println(F("ID10 customname failed!")); 
      delay(100);
    }
}

void loop() 
{

}
