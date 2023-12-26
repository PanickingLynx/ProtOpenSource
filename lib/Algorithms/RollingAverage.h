//These are our variables for the rolling average function.
const int avgWindow = 2;
int avgIndex = 0;
int avgSum = 0;
int avgBuffer[avgWindow];

//This function takes an integer and averages it with the previous value sent to it. It basically functions as a cheap low-pass filter to remove sudden loud noises and other people talking
//val - the value you want the rolling average of
//returns the rolling average of val
int rollingAverage(int val){
  avgSum = avgSum - avgBuffer[avgIndex];       // Remove the oldest entry from the sum
  avgBuffer[avgIndex] = val;           // Add the newest reading to the window
  avgSum = avgSum + val;                 // Add the newest reading to the sum
  avgIndex = (avgIndex+1) % avgWindow;   // Increment the index, and wrap to 0 if it exceeds the window size

  return avgSum / avgWindow;      // Divide the sum of the window by the window size for the result
}