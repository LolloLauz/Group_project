#include "Measurement.h"

String toJson(Measurement dataVariable) {
  JSONVar toSend;
  toSend["volt"] = dataVariable.voltage;
  toSend["corrent"] = dataVariable.current;
  toSend["potenza"] = dataVariable.power;
  toSend["energia"] = dataVariable.energy;
  toSend["frequenza"] = dataVariable.frequency;
  toSend["fattore_potenza"] = dataVariable.pf;
  toSend["millis"] =  dataVariable.milli;
  return JSON.stringify(toSend);
}

/*double scale(int analog, double min, double max) {
  double step = (max - min) / 4095.0;
  return analog * step + min;
}*/
