// This function draws a an image on the matrix.
// offset_x - The x offset for the image
// offset_y - The y offset for the image
// image[] - The image to draw on the screen. The first 3 entries are width, height and array size. The images are encoded using run length encodinbg (RLE) and are formatted as follows: first 3 bytes are RGB, last byte is how many times to repeat.
// overlay - This function uses a double buffer for seamless transitions. Setting to false will cause the display not to switch buffers, letting you overlay images on top of eachother.
// mirror - This mirrors the image if set to true, so you only need to have half the image
void drawFace(int offset_x, int offset_y, const uint32_t image[], bool overlay, bool mirror)
{
    // From our array, this extracts the metadata of our image from the first 3 entries.
    int imgWidth = image[0];
    int imgHeight = image[1];
    int arraySize = image[2];
    // This clears our frame buffer if overlay is set to false
    if (!overlay)
    {
        display.clearDisplay();
    }

    // This keeps track of which pixel in our real image we are at
    int pixel_index = 0;

    for (int i = 0; i < arraySize; i++)
    {
        // This selects the current chunk of pixels to process
        // We offset by 3 because first 3 entries are width, height and array size
        uint32_t pix = image[i + 3];

        // This unpacks the first 3 bytes into RGB values in our data chunk
        uint8_t r = (pix >> 24 & 0xFF);
        uint8_t g = (pix >> 16 & 0xFF);
        uint8_t b = (pix >> 8 & 0xFF);

        // This unpacks the last byte and tells the loop below how many times to repeat the RGB values above
        uint8_t count = pix & 0xFF;

        // This then draws those RGB values 'count' times.
        for (int j = 0; j <= count; j++)
        {
            // To get which x position in the image, we use an modulo operation which binds x between 0 and the image width. When x exceeds image width, it sets back to 0 and we increment y
            int x = (pixel_index % imgWidth);
            // To get the y position in the image, we perform integer division of the pixel's index by the width. Integer divison floors the resultant value which only increments for multiples of imgWidth.
            int y = floor(pixel_index / imgWidth);
            // We then increment our pixel counter
            pixel_index++;

            // This then draws the image to the buffer
            display.drawPixelRGB888(x + offset_x, y + offset_y, r, g, b);

            // If mirroring was enabled, draw the image on the other side of the display mirrored
            if (mirror)
            {
                display.drawPixelRGB888(offset_x + imgWidth * 2 - x, y + offset_y, r, g, b);
            }
        };
    }
    // This then shows the displays the frame buffer to the matrix
    display.showBuffer();
}