import PIL.Image as Image
import glob as glb

if __name__ == '__main__':
    f = open("./testing/images.h", "w")

    unencoded_data = []
    for filename in glb.glob("./testing/*.png"):
        image = Image.open(filename)
        data = image.load()
        f.write("const uint32_t static PROGMEM "+filename.split(".")[1].split("\\")[-1]+"[] = {"+str(image.size[0])+", "+str(image.size[1])+", ")
        for y in range(image.size[1]):
            for x in range(image.size[0]):
                imgdata = data[x, y]
                output =[imgdata[0], imgdata[1], imgdata[2]]
                unencoded_data.append(output)

        #Encode and compress the data:
        #We use 0xffffffff as a mask for since all these values are signed
        prevInt = (unencoded_data[0][0] << 24 | unencoded_data[0][1] << 16 | unencoded_data[0][2] << 8) & 0xffffffff
        counter = 1
        integer = 0
        encoded_data = []

        for item in unencoded_data[1:]:
            
            integer = (item[0] << 24 | item[1] << 16 | item[2] << 8) & 0xffffffff
            if prevInt == integer and counter < 255:
                counter+=1
            else:
                encoded_data.append(integer | (counter & 0xffffffff))
                counter = 1

            prevInt = integer

        #appends the last element
        encoded_data.append(integer | (counter & 0xffffffff))

        print(len(encoded_data))
        print(bin(encoded_data[0] & 0xffffffff))

        #Write the encoded image to a file:

        for elements in encoded_data:
            f.write("{0}, ".format(elements))
        f.write("};\n")
    f.close()