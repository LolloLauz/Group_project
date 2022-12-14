#pragma once 


#include <WiFi.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h>

#define GMT_OFFSET_SEC 3600
#define DAYLIGHT_OFFSET_SEC 3600
#define NTP_SERVER "pool.ntp.org"

extern RTC_DS3231 rtc;
extern struct tm timeinfo;
//argomento della funzione DateTime dt
void printDateTime();
void syncRTCtoNTP();
void syncTime(TimerHandle_t xTimer);
String getDateTime();
unsigned long getTimeMillis();
void sincronizzaOra(WiFiEvent_t event, WiFiEventInfo_t info);
