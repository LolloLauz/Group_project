#pragma once
#include <PZEM004Tv30.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include "Measurement.h"
#include "TimeController.h"
#include "ConnectionController.h"
#include "FlashController.h"

#define VOLT_PIN 35
#define AMPERE_ONE_PIN 32
#define AMPERE_TWO_PIN 33
#define AMPERE_THREE_PIN 25


/* Hardware Serial2 is only available on certain boards.
 * For example the Arduino MEGA 2560
*/


#define CLIENT_ID "Building0Test"

extern WiFiClient espClient;
extern PubSubClient client;

extern QueueHandle_t queue;

void readTask(TimerHandle_t xTimer);
void valueConsumer(void *pvParameters);
bool sendMqttData(Measurement measurement);
bool sendMqttData2(Measurement measurement);
