import PIL.Image as Image
import glob as glb

if __name__ == '__main__':
    f = open("./lib/FacialSprites/images.h", "w")

    for filename in glb.glob("./lib/FacialSprites/*.png"):
        unencoded_data = []
        frame = Image.open(filename)
        
        #Set to True to enable mirroring
        if False:
            newframe = Image.new(mode="RGB", size=(frame.size[0]*2, frame.size[1]))
            newframe.paste(frame, (0,0))
            newframe.paste(frame, (frame.size[0],0))
        else:
            newframe = frame.convert('RGB')

        height = newframe.size[1]
        width = newframe.size[0]

        f.write("const uint32_t static PROGMEM "+filename.split(".")[1].split("\\")[-1]+"[] = {"+str(width)+", "+str(height)+", ")
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

        print(len(encoded_data))
        print(bin(encoded_data[0] & 0xffffffff))

        #Adds in the size of the image array to the 3rd element:
        f.write("{0}, ".format(len(encoded_data)))

        #Write the encoded image to a file:
        for elements in encoded_data:
            f.write("{0}, ".format(elements))
        f.write("};\n")
    f.close()