/*
  Project: Open Source Protogen (Protogen Interactive Visor Base)
  Code by: PanickingLynx
  This Version: V1.5 (17.05.2023)

  Dear fellow furry or otherwise interested programmer,
  please read the code documentation in /doc/DOCUMENTATION.md
  before you ask me any questions at @PanickingLynx on Twitter or Telegram.

  If you can't find anything you need or if something is missing, don't hesitate to message me!

  XOXO ~ PanickingLynx
*/

//Uncomment this if your matrices are displaying errors like color shifts or flickering pixels (Mostly the case with matrices bought from AliExpress)
#define PxMATRIX_SPI_FREQUENCY 19000000

//Include Project dependencies
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <PxMatrix.h>

//Load Sprites from Headerfiles in: /lib/facialSprites (They should be automatically found with Platform IO)
//Fill these files with your own data that you have exported from LED Matrix Studio. Look into /doc/DOCUMENTATION.md on how to do this.
#include "mouth_closed_sprite.h"
#include "mouth_quiet_sprite.h"
#include "mouth_normal_sprite.h"
#include "mouth_loud_sprite.h"
#include "mouth_yelling_sprite.h"

// Set the connector pins for the LED matrices and microphone
#define MIC_PIN 12 //Sets the pin of the microphone to PIN 12
#define P_LAT 22 //Connect your matrix to these pins
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 16
//For how the ESP and matrices are connected, please consult the graphic made by @Yuri_Lynx found in /doc/ESP32-TO-P3MATRIX.pdf
//This graphic shows you how to attach circle matrices for the sides as well, but be aware that this code doesn't cover these.

#define UPPER_STARTUP "ProtOS"
#define LOWER_STARTUP "V1.5"

// Predefine functions (nessecary for PlatformIO projects)
void getMicrophoneLevel();
void speak(int microphone_level);
void isIdle(); 
void drawFace(int x_offset, int y_offset, const uint8_t pixels[]); 
void Task2code(void * pvParameters);
void waitInterruptable(int millis_target);

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//Predefining up task variables for dual core operation.
TaskHandle_t Task1;
TaskHandle_t Task2;

//Predefines for Millis timer
unsigned long previousMillis;
unsigned long currentMillis = millis();

// This defines the 'on' time of the display in use. The larger this number,
// the brighter the display. The ESP will crash if this number is too high.
uint8_t display_draw_time = 10; //10-50 is usually fine

PxMATRIX display(128, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

/*
  Predefining color palette for start-up sequence

  Add your own colors by adding a line like:

  uint16_t COLOR_NAME = display.color565(R,G,B)
  0 = No intensity
  255 = Max intensity
*/
uint16_t RED = display.color565(255, 0, 0);
uint16_t GREEN = display.color565(0, 255, 0);
uint16_t BLUE = display.color565(0, 0, 255);
uint16_t WHITE = display.color565(255, 255, 255);
uint16_t YELLOW = display.color565(255, 255, 0);
uint16_t CYAN = display.color565(0, 255, 255);
uint16_t MAGENTA = display.color565(255, 0, 255);
uint16_t BLACK = display.color565(0, 0, 0);

//Code in setup only runs once
void setup() {
    pinMode(MIC_PIN, INPUT); //Sets mode of mic pin (PIN 0) to input
    xTaskCreatePinnedToCore(
        Task2code, // Task function.
        "Task2", // name of task.
        11000, //Stack size of task
        NULL, // parameter of the task
        24, // priority of the task
        &
        Task2, // Task handle to keep track of created task
        0 // pin task to core 1
    );
    display.begin(16);
    display.setMuxDelay(2, 2, 2, 2, 2);
    display.setPanelsWidth(2);

    display.setFastUpdate(false);
    display.clearDisplay();
    display.setTextColor(BLUE);
    display.setCursor(16, 0);
    display.print(UPPER_STARTUP); 
    waitInterruptable(1000);
    display.setTextColor(WHITE);
    display.setCursor(26, 8);
    display.print(LOWER_STARTUP); 
    waitInterruptable(3000);
}

//Code in loop will run forever
void loop() {
    int microphone_level = analogRead(MIC_PIN); //Read pin value
    speak(microphone_level);
}

/*
  READ CAREFULLY:

  You probably have to adjust these values depending on the following factors:
    - Microphone placement
    - Visor design
    - Head design
    - Fan inclusion
    - Your Voice

  PLEASE DO THIS FIRST, THESE VALUES MAY BE DEFAULTS BUT ARE NOT NESSECARILY WORKING FOR YOU
*/
const int start_talking_threshhold = 2100;
const int normal_threshhold = 2200;
const int loud_threshhold = 2300;
const int yelling_threshhold = 2400;

void speak(int microphone_level) {
    if (microphone_level > start_talking_threshhold){
        previousMillis = currentMillis;
        while (currentMillis - previousMillis <= 100) {
            drawFace(0, 0, MOUTH_QUIET);
            currentMillis = millis();
        }
    } else if (microphone_level > normal_threshhold) {
        // Open mouth normally
        previousMillis = currentMillis;
        while (currentMillis - previousMillis <= 100) {
            drawFace(0, 0, MOUTH_NORMAL);
            currentMillis = millis();
        }
    } else if (microphone_level > loud_threshhold) {
        // Speak loudly
        previousMillis = currentMillis;
        while (currentMillis - previousMillis <= 100) {
            drawFace(0, 0, MOUTH_LOUD);
            currentMillis = millis();
        }
    } else if (microphone_level > yelling_threshhold) {
        // Yell
        previousMillis = currentMillis;
        while (currentMillis - previousMillis <= 100) {
            drawFace(0, 0, MOUTH_YELLING);
            currentMillis = millis();
        }
    } else {
        //Keep mouth shut
        previousMillis = currentMillis;
        while (currentMillis - previousMillis <= 100) {
            drawFace(0, 0, MOUTH_CLOSED);
            currentMillis = millis();
        }
    }
}

// Waits a time in milliseconds while still being interruptable.
// Does not halt the processor.
void waitInterruptable(int millis_target){
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= millis_target) {
        currentMillis = millis();
    }
}

//Draw the passed sprite to the screen pixel by pixel
//Screen will be drawn left to right, top to bottom
void drawFace(int x_offset, int y_offset, const uint8_t pixels[]) {
    // Draw the first panel
    int image_height = 32;
    int image_width = 64;
    int progmem_index = 0;
    for (int vertical_increment = 0; vertical_increment < image_height; vertical_increment++) {
        for (int horizontal_increment = 0; horizontal_increment < image_width; horizontal_increment++) {
            // Draw a single pixel using the next three values in the PROGMEM which represent their R, G and B values
            display.drawPixelRGB888(horizontal_increment + x_offset, vertical_increment + y_offset, pixels[progmem_index], pixels[progmem_index + 1], pixels[progmem_index + 2]);
            // Advance the progmem_index by three to get the next RGB pair
            progmem_index = progmem_index + 3;
        }
    }
    // Draw the second panel mirrored
    image_height = 32;
    image_width = 64;
    progmem_index = 0;
    for (int vertical_increment = 0; vertical_increment < image_height; vertical_increment++) {
        for (int horizontal_increment = 63; horizontal_increment > -1; horizontal_increment--) {
            // Draw a single pixel using the next three values in the PROGMEM which represent their R, G and B values
            display.drawPixelRGB888(horizontal_increment + 64, vertical_increment + y_offset, pixels[progmem_index], pixels[progmem_index + 1], pixels[progmem_index + 2]);
            // Advance the progmem_index by three to get the next RGB pair
            progmem_index = progmem_index + 3;
        }
    }
}

//Refresh the screen once every millisecond.
void Task2code(void * pvParameters) {
    for (;;) {
        delay(1);
        display.display(50);
    }
}

/*
    TODO:
    Find a way to update the matrices often enough on one core
    to not need this annoying function.
    This function is the only reason this code needs to run on ESP32.
*/