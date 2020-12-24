#include <DFRobot_sim808.h>

  DFRobot_SIM808 sim808(&Serial);
  int Powerkey = 12;
   int power()=0;

  void setup() {
  
    pinMode(Powerkey, OUTPUT);   // initialize the digital pin as an output.  
    power();                 //power on the sim808 or power down the sim808        
  
    
    //mySerial.begin(9600);
    Serial.begin(9600);

    //******** Initialize sim808 module *************
    while(!sim808.init()) {
        delay(1000);
        Serial.print("Sim808 init error\r\n");
    }

    //************* Turn on the GPS power************
    if( sim808.attachGPS())
        Serial.println("Open the GPS power success");
    else
        Serial.println("Open the GPS power failure");

  }

  void loop() {
     //************** Get GPS data *******************
     if (sim808.getGPS()) {
      Serial.print(sim808.GPSdata.year);
      Serial.print("/");
      Serial.print(sim808.GPSdata.month);
      Serial.print("/");
      Serial.print(sim808.GPSdata.day);
      Serial.print(" ");
      Serial.print(sim808.GPSdata.hour);
      Serial.print(":");
      Serial.print(sim808.GPSdata.minute);
      Serial.print(":");
      Serial.print(sim808.GPSdata.second);
      Serial.print(":");
      Serial.println(sim808.GPSdata.centisecond);
      Serial.print("latitude :");
      Serial.println(sim808.GPSdata.lat);
      Serial.print("longitude :");
      Serial.println(sim808.GPSdata.lon);
      Serial.print("speed :"); //speed in knots
      Serial.println(sim808.GPSdata.speed_kph/1.852);
      Serial.print("heading :");
      Serial.println(sim808.GPSdata.heading);
      Serial.println();

      //************* Turn off the GPS power ************
      sim808.detachGPS();
    }
    void power(void)
{
  digitalWrite(Powerkey, LOW); 
  delay(1000);               // wait for 1 second
  digitalWrite(Powerkey, HIGH);
}

  }
