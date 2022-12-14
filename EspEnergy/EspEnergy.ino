#include "ConnectionController.h"
#include "FlashController.h"
#include "MeasurementController.h"
#include "TimeController.h"
#include "LedController.h"

#define BUTTON_RESET_PIN 18

TimerHandle_t thMeasurementProducer;
TaskHandle_t thMeasurementConsumer;
TaskHandle_t thLedController;
TimerHandle_t thTimerSync;
TimerHandle_t thButtonPressed;
TimerHandle_t thCheckConnection;


void IRAM_ATTR buttonPressed()
{
  xTimerStart(thButtonPressed, 0);
}


void setup()
{
  // Setting up serial and pin modes
  Serial.begin(115200);
  
  attachInterrupt(BUTTON_RESET_PIN, buttonPressed, FALLING);

  // Configuring Flash Memory and WiFi connection
  Serial.println("Booting...");
  if (initializeSPIFFS())
  {
    InternetConfig* sdConf = readFromFile();
    if (sdConf != NULL)
    {
      conf = sdConf;
      Serial.println("Connecting from sd...");
      Serial.print("Username: ");
      Serial.println(conf->username);
      Serial.print("Password: ");
      Serial.println(conf->password);
      Serial.print("SSID: ");
      Serial.println(conf->ssid);
      if(!selectEncryptionType((wifi_auth_mode_t)3, conf->ssid, conf->username, conf->password)){
        Serial.println("Ops sempbra che qualcosa sia andato storto nella configurazione");
        conf = new InternetConfig();
        configureDevice();
        writeToFile("/test.txt", conf);
        //selectEncryptionType((wifi_auth_mode_t)3, conf->ssid, conf->username, conf->password);
      }
      
      //WiFi.disconnect();
    }
    else
    {
      conf = new InternetConfig();
      configureDevice();
      writeToFile("/test.txt", conf);
    }
  }
  else
  {
    conf = new InternetConfig();
    configureDevice();
  }
    // Configuring MQTT connection
    client.setServer(conf->broker, 1883);
    client.connect(CLIENT_ID);
    if (!client.connected() && !client.connect(CLIENT_ID)) {
        Serial.println("configurazione MQTT errata");
        conf = new InternetConfig();
        configureDevice();
    }
  // Handling all saved unsent measurements from last session
 
    
  //readMeasurementFromFile();
  syncTime(thTimerSync);

  // Setting up all tasks
  setupTasks();
}

void loop()
{
  // We delete loop tasks since it would block system
  vTaskDelete(NULL);
}

// Button Pressed Task
void onButtonPressed(TimerHandle_t xTimer)
{
  resetESP();
}

void setupTasks() {
  // ##########################
  // ### Measurements Tasks ###

  // Queue

  queue = xQueueCreate(10, sizeof(Measurement));
  if (queue == NULL)
  {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }

  // Measurement Producer
  
  thMeasurementProducer = xTimerCreate("Timer", 5000, pdTRUE, (void *)0, readTask);
  if (thMeasurementProducer == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStartFromISR(thMeasurementProducer, (BaseType_t *)0);

  // Measurement Consumer

  xTaskCreate(valueConsumer, "consumer", 4096, (void *)1, 10, &thMeasurementConsumer);
  if (thMeasurementConsumer == NULL)
  {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

  // ### End Measurements Tasks ###
  // ##############################

  // ####################
  // ### System Tasks ###

  // LED Controller
  
  xTaskCreate(ledTask, "consumer_light", 2048, NULL, 3, &thLedController);
  if (thLedController == NULL)
  {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

  // RTC Time Sync to NTP

  thTimerSync = xTimerCreate("TimerSync", 100000, pdTRUE, (void *)0, syncTime);
  if (thTimerSync == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStart(thTimerSync, 0);

  // Reset Button

  thButtonPressed = xTimerCreate("buttonTimerPress", 1000, pdFALSE, (void *)0, onButtonPressed);
  if (thButtonPressed == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }
  //Task for the Wifi connection
  //Nel momento in cui salta la connessione il task si attiva ed inizia a tentare di riconnettersi alla rete
  //nel caso in cui si sia disconnesso perche' ad esempio la rete e' saltata nel momento in cui l'access point ritorna ad essere disponibile
  //il dispositivo avendo gia una configuarzione salvata si riconnettera da solo
  //qualo'ora sia cambiato qualcosa nella configurazione di accesso alla rete bisognera manualmente riconfigurare il dispositivo
  
  WiFi.onEvent(checkConnectionStatus,SYSTEM_EVENT_STA_DISCONNECTED);
  // ### End System Tasks ###
  // ########################
}
