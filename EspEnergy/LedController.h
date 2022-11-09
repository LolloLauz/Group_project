#pragma once

#include <WiFi.h>

#include "FlashController.h"

#define RED_LIGHT_PIN 4
#define GREEN_LIGHT_PIN 1
#define BLUE_LIGHT_PIN 2

void setupLed();
void setRedLight();
void setGreenLight();
void setBlueLight();
void turnOffLed();
void setRgbColor(int red_light_value, int green_light_value, int blue_light_value);
void ledTask(void * parameter);
