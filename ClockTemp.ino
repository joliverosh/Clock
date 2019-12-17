#include <Wire.h>    // I2C-Bibliothek einbinden
#include "SPI.h"
#include "RTClib.h"  // RTC-Bibliothek einbinden

RTC_DS3231 RTC;      // RTC Module set to UTC

#include <OneWire.h>                   // OneWire-Bibliothek einbinden
#include <DallasTemperature.h>         // DS18B20-Bibliothek einbinden
#define DS18B20_PIN 4                  // Pin für DS18B20 definieren Arduino D2
OneWire oneWire(DS18B20_PIN);          // OneWire Referenz setzen
DallasTemperature sensors(&oneWire);   // DS18B20 initialisieren

#include "TM1637.h"
#define CLK 2                          // pins definitions for TM1637 and can be changed to other ports
#define DIO 3
TM1637 tm1637(CLK,DIO);
#define ON 1
#define OFF 0

#include <HCMAX7219.h>
#define NUMBEROFDRIVERS 1 <- Change this number
#define LOAD 10
HCMAX7219 HCMAX7219(LOAD);
/*PINOUT:
MODULE.....UNO/NANO.....MEGA
 6 VCC........+5V..........+5V
 9 GND........GND..........GND
 2 DIN........11...........51
 1 CS (LOAD)..10...........10
 4 CLK........13...........52
*/

void setup(void) {

  // Initialisiere I2C  
  Wire.begin();
  
  // Initialisiere RTC
  RTC.begin();


  // Serielle Ausgabe starten 
  Serial.begin(9600);

  HCMAX7219.Intensity(1, 10); // Valid values for Level are 0 (min) to 0x0F (max)

  // Prüfen ob RTC läuft  
//  if (! RTC.isrunning()) {
 //   Serial.println("RTC is NOT running!");

    // following line sets the RTC to the date & time this sketch was compiled
 //   RTC.adjust(DateTime(__DATE__, __TIME__));  
 // }
 
  sensors.begin();  // DS18B20 starten

  tm1637.init();
  tm1637.set(2);         // TYPICAL = 2, DARKEST = 0, BRIGHTEST = 7;

}

void loop(){

  DateTime now=RTC.now();   // aktuelle Zeit abrufen 
  now = now + (-5*3600);    // Restamos 5 horas al UTC

//  show_tm1637_time(now);
  show_MAX7219_time(now);
//  show_MAX72XX_time(now);

  
// show_serial_date(now);  // Datum und Uhrzeit ausgeben
// show_serial_time(now);  // Datum und Uhrzeit ausgeben

//int second;
//second = now.second();  // get seconds
//do {
//sensors.requestTemperatures();                 // Temperatursensor auslesen
//   show_tm1637_temp(sensors.getTempCByIndex(0));
//} while (second > 56);

//   show_serial_temperature(sensors.getTempCByIndex(0));  // Temperatur ausgeben
//   Serial.println("");
//   delay(1000); // 1 Sekunden warten bis zur nächsten Ausgabe
}



// Wochentag ermitteln
String get_day_of_week(uint8_t dow){ 
  
  String dows="  ";
  switch(dow){
   case 0: dows="Dom"; break;
   case 1: dows="Lun"; break;
   case 2: dows="Mar"; break;
   case 3: dows="Mie"; break;
   case 4: dows="Jue"; break;
   case 5: dows="Vie"; break;
   case 6: dows="Sab"; break;
  }
  
  return dows;
}

void show_tm1637_time(DateTime datetime){
  int hour, minute, second;
//int8_t TimeDisp[] = {0xff, 0xff, 0xff, 0xff};
int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};




  hour = datetime.hour();  // get minutes
  minute = datetime.minute();  // get minutes
  second = datetime.second();  // get seconds



  if (second % 2) tm1637.point(POINT_ON);
  else tm1637.point(POINT_OFF);

  
  TimeDisp[0] = hour / 10;
  TimeDisp[1] = hour % 10;
  TimeDisp[2] = minute / 10;
  TimeDisp[3] = minute % 10;
  
  tm1637.display(0,TimeDisp[0]);
  tm1637.display(1,TimeDisp[1]); 
  tm1637.display(2,TimeDisp[2]);
  tm1637.display(3,TimeDisp[3]);

}


void show_MAX72XX_time(DateTime datetime){
  int hour, minute, second;
int8_t TimeDisp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


  /* Clear the output buffer */
  HCMAX7219.Clear();

  hour = datetime.hour();  // get minutes
  minute = datetime.minute();  // get minutes
  second = datetime.second();  // get seconds

  if (hour < 18 ){
    HCMAX7219.Intensity(0x0F, 0);
 //    HCMAX7219.Intensity(1, 0);
  }
   else{
 //HCMAX7219.Intensity(1, 0);
    HCMAX7219.Intensity(0x0F, 0);
 
   }

  TimeDisp[0] = hour / 10;
  TimeDisp[1] = hour % 10;
  TimeDisp[2] = "";
  TimeDisp[3] = minute / 10;
  TimeDisp[4] = minute % 10;
  TimeDisp[5] = "";
  TimeDisp[6] = second / 10;
  TimeDisp[7] = second % 10;
   
  HCMAX7219.print7Seg(TimeDisp[0],8);
  HCMAX7219.print7Seg(TimeDisp[1],7);

  if (second % 2) HCMAX7219.print7Seg(":",6);
  else HCMAX7219.print7Seg(":",6);
  
  HCMAX7219.print7Seg(TimeDisp[3],5);
  HCMAX7219.print7Seg(TimeDisp[4],4);

  if (second % 2) HCMAX7219.print7Seg(":",3);
  else HCMAX7219.print7Seg(":",3);

  HCMAX7219.print7Seg(TimeDisp[6],2);
  HCMAX7219.print7Seg(TimeDisp[7],1);

  /* Send the output buffer to the display */
  HCMAX7219.Refresh();  
}


void show_MAX7219_time(DateTime datetime){
  int hour, minute, second;

  /* Clear the output buffer */
  HCMAX7219.Clear();

  hour = datetime.hour();  // get minutes
  if (hour > 12){
    hour = hour - 12;
  }
   else{
    HCMAX7219.Intensity(1, 0);
   }

  
  minute = datetime.minute();  // get minutes
  second = datetime.second();  // get seconds

  if (hour < 18){
    HCMAX7219.Intensity(0x0F, 0);
  }
   else{
    HCMAX7219.Intensity(1, 0);
   }

  
   if(hour < 10){
   HCMAX7219.print7Seg("0",8);
   HCMAX7219.print7Seg(hour,7);
  }
  else{
       HCMAX7219.print7Seg(hour,8);
  }
  HCMAX7219.print7Seg(":",6);
 
  if(minute < 10){
   HCMAX7219.print7Seg("0",5);
   HCMAX7219.print7Seg(minute,4);
  }
  else{
   HCMAX7219.print7Seg(minute,5);
    
  }
   
  HCMAX7219.print7Seg(":",3);
 
  if(second < 10){
    HCMAX7219.print7Seg("0",2);
    HCMAX7219.print7Seg(second,1);
  }
  else{
  HCMAX7219.print7Seg(second,2);  
  }
  

  /* Send the output buffer to the display */
  HCMAX7219.Refresh();  
}



// Datum und Uhrzeit ausgeben
void show_serial_time(DateTime datetime){

  
  // Stunde:Minute:Sekunde
  if(datetime.hour()<10)Serial.print(0);
  Serial.print(datetime.hour(),DEC);
  Serial.print(":");
  
  if(datetime.minute()<10)Serial.print(0);
  Serial.print(datetime.minute(),DEC);
  Serial.print(":");
  if(datetime.second()<10)Serial.print(0);
  Serial.print(datetime.second(),DEC);
  Serial.print(" ");
}


void show_serial_date(DateTime datetime){
  
  // Wochentag, Tag.Monat.Jahr
  Serial.print(get_day_of_week(datetime.dayOfTheWeek()));
  Serial.print(", ");
  
  if(datetime.day()<10)Serial.print(0);
  Serial.print(datetime.day(),DEC);
  Serial.print("/");
  if(datetime.month()<10)Serial.print(0);
  Serial.print(datetime.month(),DEC);
  Serial.print("/");
  Serial.print(datetime.year(),DEC);
  Serial.print(" ");
  }


void show_tm1637_temp(int temp){
 
//int8_t TimeDisp[] = {0xff, 0xff, 0xff, 0xff};
int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};

  TimeDisp[0] = temp / 10;
  TimeDisp[1] = temp % 10;
  TimeDisp[2] = 0x63;
  TimeDisp[3] = 12;
  
  tm1637.point(POINT_OFF);
  tm1637.display(0,TimeDisp[0]);
  tm1637.display(1,TimeDisp[1]); 
  tm1637.display(2, 0x63);
  tm1637.display(3,TimeDisp[3]);
  
  delay(1000);
}



// Temperatur ausgeben
void show_serial_temperature(float temp){
  
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" ");  // Hier müssen wir ein wenig tricksen
//  Serial.write(176);  // um das °-Zeichen korrekt darzustellen
  Serial.print("C");
}
