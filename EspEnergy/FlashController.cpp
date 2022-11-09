#include "FlashController.h"


File root;
byte *m = NULL;
uint8_t* inet_config_buf = NULL;
Measurement *misurazioni;


bool initializeSPIFFS()
{
  Serial.println(F("Inizializing FS..."));
  if (SPIFFS.begin())
  {
    Serial.println(F("done."));
  }
  else
  {
    Serial.println(F("fail."));
  }

  unsigned int totalBytes = SPIFFS.totalBytes();
  unsigned int usedBytes = SPIFFS.usedBytes();

  Serial.println("File sistem info.");

  Serial.print("Total space:      ");
  Serial.print(totalBytes);
  Serial.println("byte");

  Serial.print("Total space used: ");
  Serial.print(usedBytes);
  Serial.println("byte");

  Serial.println();
  return true;
}

void writeMeasurementToFile(Measurement m)
{
  root = SPIFFS.open("/measurement.txt", FILE_APPEND);
  if (root)
  {
    root.write((byte *)&m, sizeof(Measurement));
    root.flush();
    root.close();
  }
}

void writeToFile(String path, InternetConfig* conf)
{
  root = SPIFFS.open(path.c_str(), FILE_WRITE);
  if (root)
  {
    root.write((uint8_t *)conf, sizeof(InternetConfig));
    root.flush();
    root.close();
  }
  else
  {
    Serial.println("error opening file.txt");
  }
}

InternetConfig *readFromFile()
{
  if (!SPIFFS.exists("/test.txt")) {
    Serial.println("Config doesn't exist");
    return NULL;
  }
  root = SPIFFS.open("/test.txt");
  if (root)
  {
    inet_config_buf = (byte *)malloc(sizeof(InternetConfig));
    root.read(inet_config_buf, sizeof(InternetConfig));
    root.close();
    return (InternetConfig *)inet_config_buf;
  }
  else
  {
    Serial.println("error opening file .txt");
    return NULL;
  }
}

void readMeasurementFromFile()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int nm=numeroMisurazioni();
    unsigned long del=0;
    root = SPIFFS.open("/measurement.txt");
    if (!root) {
     Serial.println("error opening file .txt");
     return ;
    }
    Serial.print("NUMERO DELLE MISURAZIONI: ");
    Serial.println(nm);
   while (root.available())
      {
       m = (byte *)malloc(sizeof(Measurement));
       root.read(m, sizeof(Measurement));
        Measurement temp;
        temp.voltage=((Measurement *) m)->voltage;
        temp.current=((Measurement *) m)->current;
        temp.power=((Measurement *) m)->power;
        temp.energy=((Measurement *) m)->energy;
        temp.frequency=((Measurement *) m)->frequency;
        temp.pf=((Measurement *) m)->pf;
        for(int i=0;i<15;i++){
          temp.milli[i]=((Measurement *) m)->milli[i];
        }
        if(temp.milli[0]=='0'){
          Serial.println("CORREZIONE TEMPO");
          String milliScarto=String (tv_now.tv_usec/1000);
          String milli=String (tv_now.tv_sec-(nm*5));
          milli=milli+milliScarto;
          char stringa[14];
          milli.toCharArray(stringa,14);
          for(int i=0;i<15;i++){
            temp.milli[i]=stringa[i];
          }
          nm--;
        }
        sendMqttData2(temp);
      }
      root.close();
      SPIFFS.remove("/measurement.txt");
}

int numeroMisurazioni(){
  int count=0;
  root = SPIFFS.open("/measurement.txt");
  if (!root) {
    Serial.println("error opening file .txt");
    return -1;
  }
  while (root.available())
    {
      m = (byte *)malloc(sizeof(Measurement));
      root.read(m, sizeof(Measurement));
      count++;
    }
    root.close();
    return count;
}

void resetESP(){
  if (SPIFFS.exists("/test.txt"))
  {
    SPIFFS.remove("/test.txt");
  }
  if (SPIFFS.exists("/measurement.txt"))
  {
    SPIFFS.remove("/measurement.txt");
  }
  ESP.restart();
}
