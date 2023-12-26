# ProtOpenSource Documentation

## Table of Contents

1. How to add your Protogen Face
    1. C++ Header Files
    2. PROGMEM
    3. LED Matrix Studio
2. The code
    1. The include area
    2. The define area
    3. The pre-declarations
    4. The public variables
    5. setup()
        1. Setting the microphone pin (ADC Basics)
        2. Create a Core pinned task
        3. Set up the display defaults
    6. loop()
    7. getMicrophoneLevel()
    8. isSpeaking()
    9. isIdle()
    10. drawFace()
    11. Task2code()
3. Troubleshooting
4. Ending

## How to add your Protogen Face
### C++ Header Files
The files contained in ```/lib/FacialSprites``` are so calles header files.
In this project, they act as a sort of variable storage so the big blocks of binary data (PROGMEMs) won't bloat the actual ```main.cpp``` file located in ```/src/main.cpp```.

The five files represent five states of a talking mouth with different states of opening, and will advance and regress depending on the loudness of the talking done.

If you want to keep the original C++ code, do not rename these.

### PROGMEM
For example in ```mouth_closed_sprite.h```, you will find something along the lines of:

```c++
    const uint8_t static PROGMEM mouth_closed[] = {};
```

When you export your graphic in LED Matrix Studio, you will get a simmilar looking variable.
The data contents of that variable looking like ```0x0B, 0x00, 0x0F...``` etc., need to be copied into the curly braces of the variable in the ```.h``` file.

### LED Matrix Studio
This software can be obtained at [Sourceforge](https://sourceforge.net/projects/led-matrix-studio/).
I personally do not know my way around this software but there are plenty of tutorials and it also has it's own documentation.

## The code
### The include area
These various ```#include``` statements import libraries (headers) to the code. The libraries in *Lines 29-21* need to be downloaded via the PlatformIO library manager.

### The define area
These ```#define``` Statements act as a placeholder without an implicit type and their values are "inserted" at their point of reference in the code.

### The pre-declarations
A pre-declaration of functions for later redefinition. This is needed so that the C++ compiler doesn't freak out. This is a quirk of PlatformIO and would not be needed in regular ```.ino``` files edited in the Arduino IDE.

### The public variables
These variables have been declared in the most outer scope that main.cpp can give, meaning not in any functions or loops.
This gives them the unique quality of being available inside every function of ```main.cpp```.

### setup()
This function is a standard function of the Arduino library and will only run once at power-on.

It contains, as the name implies, Code to set up some things.

In our case, it contains code to set up the Microphone pin as an input, create a task for the second core of the ESP32 and set up the display.

#### Setting the microphone pin (ADC Basics)
This function call to the standard Arduino library, will configure the given pin, in our case defined in ```MIC_PIN```, as an input.
We use this pin in particular since it contains an ADC (**A**nlogue to **D**igital **C**onverter). It will take the analogue output of the microphone and convert it into values that we can work with.

#### Create a Core pinned task
This ```xTaskCreatePinnedToCore()``` function is exclusive to the ESP32, since this microcontroller has two cores available. Here we take the function ```Task2code``` and hand it over to our second core. (Or rather core 1 because our code runs on Core 2 by default)

#### Set up the display defaults
This will set up the displays and display a short message to the screen.

### loop()
This function is a standard function of the Arduino library and will run forever after power-on.

It only contains a call to the ```getMicrophoneLevel()``` function, the loop is held so minimal because it is good practice to keep your loop as clean as possible.

### getMicrophoneLevel()
This function listens to the microphone and has no parameters.

The function will read the value from our input pin, if the value exceeds the threshhold set in ```start_talking_threshhold```, the ```isSpeaking()``` function will be called.
Else, the ```isIdle()``` function will be called.

### isSpeaking()
This function takes the microphone level as a parameter and uses a chain of if-statements to decide how far the mouth will be opened.

The selected sprite will then be displayed for 100ms.

The else in the end opens the mouth a tiny bit because from the before function the minimal threshhold must already be met.

### isIdle()
This function takes no parameters and will display the closed mouth sprite for 100ms when called.

### drawFace()
This function draws the image to the screen simmilar to an old style CRT monitor.
Every value of the sprite variable is read and then written to it's corresponding LED on the Matrix.
The image will be drawn to one Matrix first and then to the other mirrored.

### Task2code()
This is a "dirty hack" to keep the displays updated. If you have a different suggestion on how to do this feel free to message me.

## Troubleshooting
If this documentation hasn't helped you fix issues on your own, feel free to message me on Twitter or Telegram @PanickingLynx .

## Ending
You have made it to the end. I hope this documentation was explained well enough even for people with low knowledge of the topics described.

And with that:

**XOXO**

~PanickingLynx
