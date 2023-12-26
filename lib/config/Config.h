// Define matrix dimensions (Defaults for P2.5 Led Matrix)
#define matrix_width 128
#define matrix_height 32

// Amount of random animations you've added (DEFAULT 4)
#define ANIM_COUNT 4

// Pin Defines
#define MIC_PIN 39  // Connect the OUT pin on the MAX4466 onto this pin.
#define BOOP_PIN 36 // This pin is used to detect booping. Connect it to a positive output of your sensor relative to ground.

// Connect these pins to your matrix. More information on wiring can be found here: https://github.com/2dom/PxMatrix
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 2
#define P_E 15
#define P_OE 16

// Uncomment this if your matrices are displaying errors like color shifts or flickering pixels (Mostly the case with matrices bought from AliExpress)
#define PxMATRIX_SPI_FREQUENCY 19900000

// This sets double buffering to be true. It allows for us to change images on the matrix seamlessly
#define double_buffer true

// @gatogatogat0 what do we call this?
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// This defines the 'on' time of the display in use. The larger this number,
// the brighter the display. The ESP will crash if this number is too high.
uint8_t display_draw_time = 50; // 10-50 is usually fine
PxMATRIX display(128, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);