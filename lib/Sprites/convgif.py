import PIL.Image as Image
from PIL import ImageSequence
import glob as glb

if __name__ == '__main__':
    f = open("./lib/FacialSprites/animations.h", "w")
    f.write("class Animation{public: const uint32_t** animation;const uint16_t* frameTimes;const int numFrames;Animation(const uint32_t** a, const uint16_t* b, const int c):animation(a), frameTimes(b), numFrames(c) {}};"+"\n\n")

    for filename in glb.glob("./lib/FacialSprites/*.gif"):
        name = filename.split(".")[1].split("\\")[-1]
        image = Image.open(filename)
        animation_array = ""
        frame_count = 0
        frame_times = ""
                
        for idx, frame in enumerate(ImageSequence.Iterator(image)):
            unencoded_data = []
            animation_array = animation_array + name + str(idx+1) + ","
            frame_count = idx + 1
            frame_times+=str(frame.info['duration'])+","


            #Set to True to enable mirroring
            if False:
                newframe = Image.new(mode="RGB", size=(frame.size[0]*2, frame.size[1]))
                newframe.paste(frame, (0,0))
                newframe.paste(frame, (frame.size[0],0))
            else:
                newframe = frame.convert('RGBA')
            height = newframe.size[1]
            width = newframe.size[0]
            
            f.write("const uint32_t PROGMEM static "+name+str(idx+1)+"[] = {"+str(width)+", "+str(height)+", ")
            for y in range(height):
                for x in range(width):
                    r, g, b = newframe.getpixel((x, y))
                    unencoded_data.append([r, g, b])

            #Encode and compress the data:
            #We use 0xffffffff as a mask for since all these values are signed
            prevInt = (unencoded_data[0][0] << 24 | unencoded_data[0][1] << 16 | unencoded_data[0][2] << 8) & 0xffffffff
            counter = 0
            integer = 0
            encoded_data = []

            for item in unencoded_data[1:]:
                
                integer = (item[0] << 24 | item[1] << 16 | item[2] << 8) & 0xffffffff
                if prevInt == integer and counter < 255:
                    counter+=1
                else:
                    encoded_data.append(prevInt | (counter & 0xffffffff))
                    counter = 0

                prevInt = integer
            
            
            #appends the last element
            encoded_data.append(prevInt | (counter & 0xffffffff))

            #Adds in the size of the image array to the 3rd element:
            f.write("{0}, ".format(len(encoded_data)))

            #Write the encoded image to a file:
            for elements in encoded_data:
                f.write("{0}, ".format(elements))

            f.write("};\n")

        #Creates a pointer to our animation arrays
        f.write("\nconst uint32_t* ANI_"+name+"[] = {"+animation_array+"};\n")

        #Creates an array with the frametimes
        f.write("const uint16_t FRAME_TIMES_"+name+"[] = {"+frame_times+"};\n")

        #Creates the Animation class to use:
        f.write("Animation "+name+"(ANI_"+name+",FRAME_TIMES_"+name+","+str(frame_count)+");\n\n")

    f.close()