/*
  Project: Open Source Protogen (Protogen Interactive Visor Base)
  Code by: PanickingLynx, Katxe
  This Version: V2.0b (17.05.2023)

  Dear fellow furry or otherwise interested programmer,
  please read the code documentation in /doc/DOCUMENTATION.md
  before you ask us any questions at @PanickingLynx or @Katxe on Twitter or Telegram.

  If you can't find anything you need or if something is missing, don't hesitate to message me!

  XOXO ~ PanickingLynx, Katxe
*/

// Include Project dependencies
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <PxMatrix.h>

#include "Imports.h"

void Task2code(void *pvParameters);

// Predefining up task variables for dual core operation.
TaskHandle_t Task1;
TaskHandle_t Task2;

// --------------------------------------------------------------------------------------------------------------------------------

// Code in setup only runs once
void setup()
{
    // This sets up our mic and boop pins, letting us read data values through them
    pinMode(MIC_PIN, INPUT);
    pinMode(BOOP_PIN, INPUT);

    // This sets up dual core processing
    xTaskCreatePinnedToCore(
        Task2code, // Task function.
        "Task2",   // name of task.
        11000,     // Stack size of task
        NULL,      // parameter of the task
        24,        // priority of the task
        &Task2,    // Task handle to keep track of created task
        0          // pin task to core 1
    );

    // This sets up Serial as well as the display settings and the booting screen.
    Serial.begin(115200);
    display.begin(16);
    display.display(display_draw_time);
    display.setFastUpdate(false);
    display.setBrightness(255);
    display.setPanelsWidth(2);
    display.setTextColor(display.color565(0, 0, 255));
    display.setCursor(16, 0);
    display.print("BOOTING UP....");
    display.setTextColor(display.color565(255, 255, 255));
    display.setCursor(16, 8);
    display.print("ProtOpenSource V2.0");
    display.showBuffer();
    waitInterruptable(3000);
    display.showBuffer();
    display.clearDisplay();
}



// This is our main loop
void loop()
{
    // This first checks if a boop is occuring. If so, it will play the boop animation defined in boopSensor and not run the microphone check.
    if (!boopSensor())
    {
        getMicrophoneLevel();
    }
}


// This refreshes the display every 1ms to the currently selected frame buffer
void Task2code(void *pvParameters)
{
    for (;;)
    {
        delay(1); // For some odd fucking reason we can only use delay here because the second core does not keep track of it's own processor time or atleast it's not easily accessable
        display.display(display_draw_time);
    }
}

// TODO: ADD IN BLUETOOTH