// This is the threshold value that will trigger the boop animation when the signal on that pin exceeds or equals to.
uint16_t boopThreshold = 2500;
// This function checks if the protogen has been booped by comparing the boop signal to the threshold.
// It returns true when booped and false if not booped.
bool boopSensor()
{
    if (analogRead(BOOP_PIN) >= boopThreshold)
    {
        // We set this variable up to randomly pick from a pool of 4 animations.
        // random(x,y) excludes the upper value so we set it to 4 instead of 3.
        int rand = random(0, ANIM_COUNT);

        // We use a switch case to play the animation depending on which integer rand selects.
        // If you add more animations, be sure to increase the value for ANIM_COUNT.
        switch (rand)
        {
        case 0:
            playAnimation(0, 0, bk, true);
            break;
        case 1:
            playAnimation(0, 0, bk2_, true);
            break;
        case 2:
            playAnimation(0, 0, bk3_, true);
            break;
        case 3:
            playAnimation(0, 0, bk4_, true);
            break;
        }
        return true;
    }
    else
    {
        return false;
    }
}