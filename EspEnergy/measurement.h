#pragma once

#include <RTClib.h>
#include <Arduino_JSON.h>

typedef struct {
  float voltage;
    float current;
    float power;
    float energy ;
    float frequency;
    float pf;
  char milli[14];
} Measurement;

String toJson(Measurement dataVariable);
//double scale(int analog, double min, double max);
