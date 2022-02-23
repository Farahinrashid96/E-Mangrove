//GPRS
#include "FVlib_Arduino_SIM900_Ubidots_Client.h"

//Husky
#include "HUSKYLENS.h"     
#include "SoftwareSerial.h"

//GY86
#include "Wire.h"
#include "I2Cdev.h" 
#include "MPU6050.h" 
#include <MS5611.h>

//Fuel Gauge Battery
#include "DFRobot_MAX17043.h" 

//Battery
#ifdef __AVR__
  #define ALR_PIN       2
#else
  #define ALR_PIN       D2
#endif

#define PRINT_INTERVAL        2000
#define BAT_PERCENTAGE        32

DFRobot_MAX17043        gauge;
uint8_t       intFlag = 0;

//GPRS
#define ubidotsToken "BBFF-5qbhP3sJqW45xEW3jHIwtn1viOyaTP"
#define ubidotsVariableID1 "6152c6ccc7b3e92875d0a5b1"     //RSSI GPRS
#define ubidotsVariableID2 "616f986f0a232a167e2bbea4"     //Wave Height GY86
#define ubidotsVariableID3 "616f98650a232a167e2bbea3"     //Wave Period GY86
#define ubidotsVariableID4 "6152c5ff981c9220993d595c"     //Wave Power GY86
#define ubidotsVariableID5 "6152c612981c9220993d595d"     //Battery Level
#define ubidotsVariableID6 "61ef5da486898e3a3e2bf6f6"     //Bottle1
#define ubidotsVariableID7 "61ef5e5e0d80b03a29cd9400"     //Non2
#define ubidotsVariableID8 "61ef5e72ce1d35dbcd5cc7d5"     //Can3
#define ubidotsVariableID9 "61ef5e8bfd12b373af079726"     //Bottle4
#define ubidotsVariableID10 "61ef5ea0f3555aa7e84055a3"    //Can5
#define ubidotsVariableID11 "61ef5ebc86898e3a3e2bf712"    //Bottle6
#define ubidotsVariableID12 "61ef5ef486898e3ab30057af"    //Plastic7
#define ubidotsVariableID13 "61ef5f24ce1d35dbcb83fea6"    //Plastic8
#define ubidotsVariableID14 "61ef5f3bfd12b373af07972f"    //Plastic9
#define ubidotsVariableID15 "61ef5f5ace1d35de14777e91"    //Plastic10
sim900 modem(ubidotsToken);

//GY86
MPU6050 mpu;
MS5611 baro;
long pressure;
double altitude, min_height, max_height, wave_height, mid_point, smudge_factor, wave_power;
byte escaped, started;
unsigned long period_start, period_end;
float avg_period = -1;

//GPRS
int batec = 0;

//Husky
HUSKYLENS huskylens;
SoftwareSerial mySerial(11, 12); // RX, TX / HUSKYLENS green-yellow line >> Pin 11; blue-choco line >> Pin 12
void printResult(HUSKYLENSResult result);
void husky(HUSKYLENSResult result);

//Battery
void interruptCallBack()
{
  intFlag = 1;
}

void setup() {

/***********************************Initialize Part********************************/ 
  Serial.begin(9600);       //portSim900.begin(9600);
  mySerial.begin(9600);    //Huskylens
  modem.setModemOnline(); //GPRS
  Wire.begin();

/***********************************GY86 Code********************************/  
 Serial.println("Initializing I2C devices...");
  mpu.setI2CMasterModeEnabled(false);
  mpu.setI2CBypassEnabled(true) ;
  mpu.setSleepEnabled(false);
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  while (!baro.begin(MS5611_ULTRA_HIGH_RES)) {
    Serial.println("MS5611 connection unsuccessful, check wiring!");
    delay(500);
    }
    
/***********************************Huskylens Code********************************/    
   while (!huskylens.begin(mySerial))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    } 
}

void loop() {
  /***********************Battery Level Code********************/
  while(gauge.begin() != 0) {
    Serial.println("gauge begin faild!");
    delay(2000);
  }
  delay(2);
  Serial.println("gauge begin successful!");
  
  /************************GY86 Code*************************/
  double  height = 10.0;
  double period = 20.0;
  double power = 30.0;

  height = GY86WaveHeight();
  period = GY86WavePeriod();
  power = GY86WavePower();

  /************************************Send Data To Ubidots Code**************************************/  
  Serial.print("Post Result: ");
  Serial.println(modem.sendData(modem.getSignalStrengthRSSI(3), ubidotsVariableID1, false));
  Serial.println(modem.sendData(height, ubidotsVariableID2, false));
  Serial.println(modem.sendData(period, ubidotsVariableID3, false));
  Serial.println(modem.sendData(power, ubidotsVariableID4, false));
  Serial.println(modem.sendData(gauge.readPercentage(), ubidotsVariableID5, false));

  /**************************************Huskylens Code****************************************/
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else
    {
        Serial.println(F("###########"));
        while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
            delay(1000);
            husky(result); //added code
            
        }    
    }
}

/************************************Huskylens Code*************************************/
 void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
        delay(1000);
    }
    else if (result.command == COMMAND_RETURN_ARROW){
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
        delay(1000);
    }
    else{
        Serial.println("Object unknown!");
         delay(1000);
    }
 }

 void husky(HUSKYLENSResult result){
    if (result.ID == 1){
       Serial.println("Plastic");
       Serial.println(modem.sendData(result.ID,ubidotsVariableID6,false));
       delay(1000);
    }
     else if (result.ID == 2){
      Serial.println("Can");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID7, false));
      delay(1000);
    }
    else if (result.ID == 3){
      Serial.println("Glass Bottle");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID8, false));
      delay(1000);
    }
     else if (result.ID == 4){
      Serial.println("Glass Bottle");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID9, false));
      delay(1000);
    }
     else if (result.ID == 5){
      Serial.println("Plastic Bottle");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID10, false)); 
      delay(1000);
    }
     else if (result.ID == 6){
      Serial.println("Can");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID11, false));
      delay(1000);
    }
     else if (result.ID == 7){
      Serial.println("Plastic");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID12, false));
      delay(1000);
    }
     else if (result.ID == 8){
      Serial.println("Plastic");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID13, false)); 
      delay(1000);
    }
     else if (result.ID == 9){
      Serial.println("Plastic");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID14, false));
      delay(1000);
    }
      if (result.ID == 10){
      Serial.println("Plastic");
      Serial.println(modem.sendData(result.ID, ubidotsVariableID15, false));
      delay(1000);
    }
     else {
      Serial.println("Unknown");
     }
  }

/****************************GY86 Code******************************/
 float GY86WaveHeight()
 {
  unsigned long start_time = millis();
  pressure = baro.readPressure(true);
  altitude = baro.getAltitude(pressure);
  max_height = altitude;
  min_height = altitude;

  //  for 5 seconds
  while(millis() - start_time < 5000){
    pressure = baro.readPressure(true);
    altitude = baro.getAltitude(pressure);
    if (altitude < min_height) min_height = altitude;
    if (altitude > max_height) max_height = altitude;
  }
  mid_point = (max_height + min_height)/2.0;
  wave_height = (max_height - mid_point) / 2.0;
  smudge_factor = wave_height * 0.15;
  //Serial.print("Wave Height m: "); Serial.print(wave_height);
  return wave_height;
 }
 
  float GY86WavePeriod() 
  {
   unsigned long start_time = millis();
   start_time = millis();
  //  for 5 seconds 
  while(millis() - start_time < 5000){
    pressure = baro.readPressure(true);
    altitude = baro.getAltitude(pressure);
       //if within a range of 30% of wave height from the mid point
       //start the timer else stop it
    if (altitude < mid_point + smudge_factor && altitude > mid_point - smudge_factor){
      if ( !started){
        period_start = millis();
        started = true;
      }
      else{
        if ( escaped ){
          // if it has started and escaped and is now returning
          period_end = millis();
          started = false;
          escaped = false;
          // if the variable has already been assigned
          // use its previous value and new value to work out avg
          if(avg_period != -1){
            avg_period = (avg_period + (period_end-period_start)*2)/ 2.0;
          }
          // assign it
          else{
            avg_period =  (period_end-period_start)*2; 
          }

        }
      }
    }
    else{
      escaped = true;
    } 
  }
    return avg_period/1000;
  }

 float GY86WavePower()
 {
   float height = 10.0;
   float period = 20.0;

   height = GY86WaveHeight();
   period = GY86WavePeriod();
   
   
   wave_power = (0.5 * height * height * period);
   return wave_power; 
 }
