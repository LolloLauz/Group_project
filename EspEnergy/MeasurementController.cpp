#include "MeasurementController.h"
#if defined(ESP32)
//RX 16 
//TX 17
PZEM004Tv30 pzem(Serial2, 16, 17);
#else
PZEM004Tv30 pzem(Serial2);
#endif
WiFiClient espClient;
PubSubClient client(espClient);

QueueHandle_t queue;
void readTask(TimerHandle_t xTimer)
{
  // Measurement Production
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();
  //ricavo la data tramite i millisecondi
  //il problema per la gestione in modo corretta della data viene scaricato a valle 
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  String tempmilliScarto=String (tv_now.tv_usec/1000);
  String tempmilli=String (tv_now.tv_sec);
  //i millisecondi li otterrei tramite un int64 essendo cosi grande avevo problemi con il cast per poterlo trasformare in stringa
  //quindi ho preso la parte dei secondi senza trasformarla in millisecondi e ci ho aggiunto i nanosecondi facendo una concatenazione di stringhe 
  tempmilli=tempmilli+tempmilliScarto;
  if(tv_now.tv_sec<1650000000){
    Serial.println("tempo non sincronizzato");
    tempmilli="0000000000000";
  }
  char milli[14];
  tempmilli.toCharArray(milli,14);
  Measurement misura = {voltage, current, power, frequency,pf};
  for (int i=0;i<15;i++){
    misura.milli[i]=milli[i];
  }
  
  
  
  // Sending to FreeRTOS Queue
  if (xQueueSendFromISR(queue, &misura, NULL))
  {
    Serial.print("Object sent");
  } else
  {
    Serial.println("Queue is full");
  }
}

// Measurement Consumer Task
void valueConsumer(void *pvParameters)
{
  configASSERT(((uint32_t)pvParameters) == 1);

  // Measurement Retrieval
  Measurement received;
  
  for (;;)
  {
    if (xQueueReceive(queue, &received, (TickType_t)100) == pdPASS)
    {
      // Measurement Usage
      if (!sendMqttData(received)) 
      {
        writeMeasurementToFile(received);
      }
    }
  }
}

bool sendMqttData(Measurement measurement)
{
  if (!client.connected() && !client.connect(CLIENT_ID)) 
  {
    Serial.println(" Publish failed");
    return false;
  }else{
    Serial.print("Topic is: ");
    Serial.print(conf->topic);
    //controllo se sono prensenti misurazioni nel vettore delle misurazioni offline
    if(numeroMisurazioni()!=0){
      readMeasurementFromFile();
    }
    if (client.publish(conf->topic, toJson(measurement).c_str())) 
    {
      Serial.println(" Publish ok");
      return true;
    }   else
    {
      Serial.println(" Publish failed");
      return false;
    }
  }
}

bool sendMqttData2(Measurement measurement)
{
    if (client.publish(conf->topic, toJson(measurement).c_str())) 
    {
      Serial.println(" Publish ok");
      return true;
    }   else
    {
      Serial.println(" Publish failed");
      return false;
    }
}
