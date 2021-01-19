// Program to demonstrate the MD_Parola library
//
// Simplest program that does something useful - Hello World!
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <ThreeWire.h>  //Library for the clock
#include <RtcDS1302.h>  //Library for the clock
#include <SPI.h>
#include "Parola_Fonts_data.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   12
#define DATA_PIN  11
#define CS_PIN    10

#define CLOCK_RSTPIN 2                       // DS1302 Chip Enable pin
#define CLOCK_DATPIN 3                       // DS1302 Input/Output pin
#define CLOCK_CLKPIN 4                       // DS1302 Serial Clock pin

#define SPEED_TIME  25
#define PAUSE_TIME  10

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

ThreeWire myWire(CLOCK_DATPIN, CLOCK_CLKPIN, CLOCK_RSTPIN); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

char dateString[20];
char timeString[9];
char buff[20];

void setup(void)
{
  P.begin();
  P.setFont(fontArabic);
  P.setIntensity(2);
  P.displayClear();
  P.setTextAlignment(PA_CENTER);

  Serial.begin(9600);
  
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  P.displayText(buff, PA_CENTER, 25, 1000, PA_PRINT, PA_NO_EFFECT);
}

void loop(void)
{
  RtcDateTime now = Rtc.GetDateTime();
  snprintf(timeString, sizeof(timeString), "%02d:%02d", now.Hour(), now.Minute());
  String month = monthAsString(now.Month());
  snprintf(dateString, sizeof(dateString), "%s %02d", month.c_str(), now.Day());
  static uint8_t nMessage = 0;
  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    switch (nMessage)
    {
      case 0:
        P.displayText(timeString, PA_CENTER, 50, 1000, PA_FADE, PA_FADE);
        nMessage++;
        break;
      case 1:
        P.displayText(dateString, PA_CENTER, 50, 3000, PA_FADE, PA_FADE);
        nMessage = 0;
        break;
    }
    P.displayReset();  // tell Parola we have a new animation
  }
}

String monthAsString(int month) {
  switch (month) {
    case 1:  return "Jan";
    case 2:  return "Feb";
    case 3:  return "Mar";
    case 4:  return "Apr";
    case 5:  return "May";
    case 6:  return "Jun";
    case 7:  return "Jul";
    case 8:  return "Aug";
    case 9:  return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
  }
  return "Error";
}

void delayMin(int number, int ms) {
  for(int i = 0; i < number; i++){
    delay(ms);
  }
}
