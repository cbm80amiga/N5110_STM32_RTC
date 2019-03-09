/*
  STM32 RTC example
  (c)2019 Pawel A. Hernik
  YouTube video: 
  https://youtu.be/Vtd0p4xMSGI
*/

// *** CONNECTIONS ***
/*
 N5110 LCD pinout from left:
 #1 RST      - PA0
 #2 CS/CE    - PA4
 #3 DC       - PA1
 #4 MOSI/DIN - PA7
 #5 SCK/CLK  - PA5
 #6 VCC      - 3.3V
 #7 LIGHT    - 200ohm to GND
 #8 GND

 STM32 SPI1 pins:
  PA4 CS1
  PA5 SCK1
  PA6 MISO1
  PA7 MOSI1
*/

#include <RTClock.h>
RTClock rtclock(RTCSEL_LSE);
tm_t curTime;

#define N5110_RST       PA0
#define N5110_CS        PA4
#define N5110_DC        PA1

// define USESPI in LCD driver header for HW SPI version
#include "N5110_SPI.h"
#if USESPI==1
#include <SPI.h>
#endif
N5110_SPI lcd(N5110_RST, N5110_CS, N5110_DC); // RST,CS,DC

#include "c64enh_font.h"
#include "term9x14_font.h"

const char *months[] = {"???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char *delim = " :";
char bld[40];

uint8_t str2month(const char * d)
{
  uint8_t i = 13;
  while((--i) && strcmp(months[i], d));
  return i;
}

void setBuildTime(tm_t & mt)
{
  // Timestamp format: "Mar 3 2019 12:34:56"
  snprintf(bld,40,"%s %s\n", __DATE__, __TIME__);
  char *token = strtok(bld, delim);
  while(token) {
    int m = str2month((const char*)token);
    if(m>0) {
      mt.month = m;
      token = strtok(NULL, delim);  mt.day = atoi(token);
      token = strtok(NULL, delim);  mt.year = atoi(token) - 1970;
      token = strtok(NULL, delim);  mt.hour = atoi(token);
      token = strtok(NULL, delim);  mt.minute = atoi(token);
      token = strtok(NULL, delim);  mt.second = atoi(token);
    }
    token = strtok(NULL, delim);
  }
  snprintf(bld,40,"Build: %02d-%02d-%02d %02d:%02d:%02d\n",mt.year+1970,mt.month,mt.day,mt.hour,mt.minute,mt.second); Serial.println(bld);
  rtclock.setTime(mt);
}
//-----------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  rtclock.breakTime(rtclock.now(), curTime);
  if(curTime.year+1970<2019) setBuildTime(curTime);  //  <2019 - invalid year
  lcd.init();
  lcd.clrScr();
}

char buf[25];
char *dowTxt[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "??"};
char *dowLongTxt[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "??"};

void showClock(int seconds)
{
  rtclock.breakTime(rtclock.now(), curTime);
  lcd.setFont(Term9x14);
  if(seconds)
    snprintf(buf, 25, " %d:%02d:%02d ", curTime.hour, curTime.minute, curTime.second);
  else
    snprintf(buf, 25, " %d:%02d ", curTime.hour, curTime.minute);
  lcd.printStr(ALIGN_CENTER, 1, buf);
  lcd.setFont(c64enh);
  snprintf(buf, 25, "% 02d.%02d.%d ", curTime.day, curTime.month, curTime.year+1970);
  lcd.printStr(ALIGN_CENTER, 4, buf);
  snprintf(buf, 25, "  %s  ", dowLongTxt[curTime.weekday]);
  lcd.printStr(ALIGN_CENTER, 5, buf);
}

void loop()
{
  showClock(1);
}

