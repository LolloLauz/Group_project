#include "TimeController.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

//RTC_DS3231 rtc;

unsigned long epochTime; 


// SCL 22 SCK 21
// argomento della funzione DateTime dt
void printDateTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  Serial.println("il valore di now");
  Serial.println(epochTime);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
}

void syncRTCtoNTP() {
  Serial.println("Sono in timeController.c e sincronizzo il tempo");
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("dispositivo non connesso sincronizzazione errata");
  }
}

// Time Sync Task
void syncTime(TimerHandle_t xTimer)
{
  syncRTCtoNTP();
}

String getDateTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  return String (timeinfo.tm_mday) + "/" + String (timeinfo.tm_mon+1) + "/" + String (timeinfo.tm_year+1900)+ "/" + String (timeinfo.tm_hour)+ "/" + String (timeinfo.tm_min)+ "/" + String (timeinfo.tm_sec);
}

unsigned long getTimeMillis(){
  time_t milli;
  time(&milli);
  epochTime=milli;
  Serial.println("-------");
  Serial.println(epochTime);
  return epochTime;

}
void sincronizzaOra(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Evento di riconnessione o connessione ad una rete");
    Serial.println("Sincronizzo l'orario");
    syncRTCtoNTP();
}
