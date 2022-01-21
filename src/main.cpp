/*
  Project: Open Source Protogen (Protogen Interactive Visor Base)
  Code by: PanickingLynx
  This Version: V1 (20.01.2022)

  Dear Fellow Furry or otherwhise interested Programmer,
  please read the Code documentation in /doc/DOCUMENTATION.pdf
  before you ask me any questions at @PanickingLynx on Twitter or Telegram.
  (Which is not yet possible so feel free to message)

  If you can't find anything you need there or you find that something is missing, don't hesitate to message me!

  XOXO ~ PanickingLynx
*/

//Include Project dependencies
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <PxMatrix.h>

//Load Sprites from Headerfiles in: /lib/FacialSprites/
//Fill these files with your own data that you have exported from LED Matrix Studio
#include <mouth_closed_sprite.h>
#include <mouth_quiet_sprite.h>
#include <mouth_normal_sprite.h>
#include <mouth_loud_sprite.h>
#include <mouth_yelling_sprite.h>

#define MIC_PIN 12 //Sets the Pin of the Microphone to PIN 12
#define P_LAT 22 //Connect your matrix to these pins !!!!!
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2

// Define Matrix dimensions
#define matrix_width 128
#define matrix_height 32

void getMicrophoneLevel(); //PREDEFINE OF FUNCTION
void isSpeaking(int microphoneLevel); //PREDEFINE OF FUNCTION
void isIdle(); //PREDEFINE OF FUNCTION
void drawFace(int x_offset, int y_offset, const uint8_t pixels[]); //PREDEFINE OF FUNCTION
void Task2code( void * pvParameters ); //PREDEFINE OF FUNCTION

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//Predefining up Task variables for Dual Core operation.
TaskHandle_t Task1; 
TaskHandle_t Task2;

//Predefines for Millis timer
unsigned long previousMillis; 
unsigned long currentMillis = millis();

//Voice threshholds
/*
  READ CAREFULLY:

  You probably have to adjust these values depending on the following factors:
    - Microphone placement
    - Visor design
    - Head design
    - Fan inclusion
    - Your Voice

  You can find a calibration sketch at: github.com/PanickingLynx/ProtoMicrophoneCalibration

  PLEASE DO THIS FIRST, THESE VALUES MAY BE DEFAULTS BUT ARE NOT NESSECARILY WORKING FOR YOU
*/
const int start_talking_threshhold = 2100;
const int normal_threshhold = 2200;
const int loud_threshhold = 2300;
const int yelling_threshhold = 2400;

// This defines the 'on' time of the display in use. The larger this number,
// the brighter the display. If too large the ESP will crash
uint8_t display_draw_time = 20; //10-50 is usually fine

// PxMATRIX display(matrix_width,matrix_height,P_LAT, P_OE,P_A,P_B,P_C);
//PxMATRIX display(128,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
PxMATRIX display(128, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

/*
  Predefining Color Palette

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
void setup(){
  pinMode(MIC_PIN, INPUT); //Sets mode of mic pin (PIN 0) to input
  xTaskCreatePinnedToCore(
    Task2code,    // Task function.
    "Task2",      // name of task.
    11000,        //Stack size of task
    NULL,         // parameter of the task
    24,           // priority of the task
    &Task2,       // Task handle to keep track of created task
    0             // pin task to core 1
  );          
  delay(500);

  display.begin(16);
  display.setMuxDelay(2, 2, 2, 2, 2);
  display.setPanelsWidth(2);

  display.setFastUpdate(false);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(16, 0);
  display.print("ProtOpenSource"); //CHANGE THIS FOR DIFFERENT UPPER STARTUP TEXT
  delay(1000);
  display.setTextColor(WHITE);
  display.setCursor(26, 8);
  display.print("V1"); //CHANGE THIS FOR DIFFERENT LOWER STARTUP TEXT
  delay(3000);
}

//Code in loop will run forever
void loop(){
    getMicrophoneLevel(); //Constantly listen to the microphone
}

// Listens in on the microphone pin.
void getMicrophoneLevel(){
  int microphoneLevel = analogRead(MIC_PIN); //Read Pin value and subtract rough noise of mic
  if (microphoneLevel > start_talking_threshhold){ //If more than 200 on level then speak
    isSpeaking(microphoneLevel);
  }else{ //Else shut up
    isIdle();
  }
}

void isSpeaking(int microphoneLevel){
  if (microphoneLevel > normal_threshhold){
    // Open mouth normally
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= 100){
      drawFace(0, 0, mouth_normal);
      currentMillis = millis();
    }
  }
  else if(microphoneLevel > loud_threshhold){
    // Yell a bit
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= 100){
      drawFace(0, 0, mouth_loud);
      currentMillis = millis();
    }
  }
  else if(microphoneLevel > yelling_threshhold){
    // PROTOPOSSUM GO AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= 100){
      drawFace(0, 0, mouth_yelling);
      currentMillis = millis();
    }
  }
  else{
    //Open mouth tiny bit.
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= 100){
      drawFace(0, 0, mouth_quiet);
      currentMillis = millis();
    }
  }
}

//Show an Idle animation
void isIdle(){
    drawFace(0, 0, mouth_closed);
}

//Draw the Passed sprite to the Screen pixel by pixel
//Screen will be drawn left to right, top to bottom
void drawFace(int x_offset, int y_offset, const uint8_t pixels[]){
  int imageHeight = 32;
  int imageWidth = 64;
  int counter = 0;
  for (int vertical_increment = 0; vertical_increment < imageHeight; vertical_increment++)
  {
    for (int horizontal_increment = 0; horizontal_increment < imageWidth; horizontal_increment++)
    {

      display.drawPixelRGB888(horizontal_increment + x_offset, vertical_increment + y_offset, pixels[counter], pixels[counter + 1], pixels[counter + 2]);

      counter = counter + 3;
    }
  }
  imageHeight = 32;
  imageWidth = 64;
  counter = 0;
  for (int vertical_increment = 0; vertical_increment < imageHeight; vertical_increment++)
  {
    for (int horizontal_increment = 63; horizontal_increment > -1; horizontal_increment--)
    {

      display.drawPixelRGB888(horizontal_increment + 64, vertical_increment + y_offset, pixels[counter], pixels[counter + 1], pixels[counter + 2]);

      counter = counter + 3;
    }
  }
}

//Refresh the screen once every millisecond.
void Task2code( void * pvParameters ) {  
  for (;;) {
    delay(1);
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= 1)
    {
      currentMillis = millis();
    }
    display.display(50); //was 60
  }
}
