#pragma once

#include <RTClib.h>
#include <Arduino_JSON.h>

typedef struct {
  int volt;
  int ampere_one;
  int ampere_two;
  int ampere_three;
  char milli[14];
} Measurement;

String toJson(Measurement dataVariable);
double scale(int analog, double min, double max);
