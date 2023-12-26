// These variables keep track of time for the facial animation loop
unsigned long blinkMillis;

// These are the activation thresholds for the talking animations to play. You will most likely need to change these values depending on mic position, manufacturer, etc.
// To change, I recommend using Serial.println(microphoneLevel) and viewing the serial plotter.
const int talk_threshold = 300;
const int loud_threshhold = 600;
const int yelling_threshhold = 1500;

// This defines the pointer array to our facial animations which will be used in getMicrophoneLevel()
// With this current setup, index 0-4 is for talking, 5-9 is for loud talking and 10-14 is for yell
const uint32_t *talkSelector[] = {
    talk1, talk2, talk3, talk4, talk5,
    loud1, loud2, loud3, loud4, loud5,
    yell1, yell2, yell3, yell4, yell5};

// This function plays the speaking animations.
// When it selects the face, it keeps playing it for 110-200ms. Every 5000-1000ms, it plays a blink animation.
// picker - which face to select from our talkSelector[] array
void isSpeaking(uint8_t picker)
{
    display.clearDisplay();
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= random(10, 200))
    {
        if (currentMillis - blinkMillis >= random(5000, 10000))
        {
            drawFace(0, 0, blink, true, true);
            blinkMillis = currentMillis;
        }
        else
        {
            drawFace(0, 0, talkSelector[picker], true, true);
        }
        waitInterruptable(100);
        currentMillis = millis();
    }
}

// This displays our idle animation for when no speaking is detected. Works the same way as isSpeaking() minus the face picker variable.
void isIdle()
{
    display.clearDisplay();
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= random(10, 200))
    {
        if (currentMillis - blinkMillis >= random(2000, 10000))
        {
            drawFace(0, 0, blink, true, true);
            blinkMillis = currentMillis;
        }
        else
        {
            drawFace(0, 0, normal, true, true);
        }
        waitInterruptable(100);
        currentMillis = millis();
    }
}

// This function listens to the mic signal and based off of that, plays a facial animation for the given sound level.
void getMicrophoneLevel()
{
    // This is the baseline signal you get from your microphone with no sound. You will have to measure this value using the method described above
    const int baseline = 1900;
    // Tracks which face has been picked to render
    uint8_t facePicker = 0;
    // This reads from the mic pin and substract the baseline to end up with the sound's waveform.
    // Because we only care about loudness for selecting which speaking animation to play, we absolute value the baseline
    // Then, we apply a rolling average to the signal as a low-pass filter
    int microphoneLevel = rollingAverage(abs(analogRead(MIC_PIN) - baseline)); // Read pin value

    // Depending on how large the mic signal is, it picks a facial animation to play for the given loudness range.
    // Make sure to order the if else statements from largest to smallest or else it'll just play only the smallest value
    if (microphoneLevel > yelling_threshhold)
    {
        facePicker = random(10, 15);
    }
    else if (microphoneLevel > loud_threshhold)
    {
        facePicker = random(5, 10);
    }
    else if (microphoneLevel > talk_threshold)
    {
        facePicker = random(0, 5);
    }

    // If the mic signal is greater than the talking threshold, it then uses the face picked by the code above to play the speaking animation.
    // If it's less than the talking threshold, it plays the idle face animation.
    if (microphoneLevel > talk_threshold)
    {
        isSpeaking(facePicker);
    }
    else
    {
        isIdle();
    }
}

