#include "Arduino.h"

//Predefines for Millis timer
unsigned long previousMillis;
unsigned long currentMillis = millis();

// Waits a time in milliseconds while still being interruptable.
// Does not halt the processor.
void waitInterruptable(int millis_target){
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= millis_target) {
        currentMillis = millis();
    }
}