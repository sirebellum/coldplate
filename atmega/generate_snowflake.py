from PIL import Image
import numpy as np

def image_to_boolean_array(image_path, output_shape=(128, 64), threshold=128, output_file="output_image.bmp"):
    # Open the image and convert to grayscale
    image = Image.open(image_path).convert("L")
    
    # Resize the image to the desired output shape
    image_resized = image.resize(output_shape)

    # Apply threshold to convert the grayscale image to black and white
    image_bw = image_resized.point(lambda x: 255 if x < threshold else 0, '1')

    # Convert the image to a NumPy array
    boolean_array = np.array(image_bw)

    # Save the resulting black-and-white image
    image_bw.save(output_file)

    # Generate formatted output for pasting into ATmega328p code
    formatted_array = []
    for row in boolean_array:
        byte = []
        for pixel in row:
            byte.append('1' if pixel else '0')
        formatted_array.append(''.join(byte))

    # Print formatted array for ATmega328p code
    # print("const bool image_array[SPLASH_HEIGHT][SPLASH_WIDTH] PROGMEM = {{".format(output_shape[1], output_shape[0]))
    # for line in formatted_array:
    #     print("    {" + ", ".join(line) + "},")
    # print("};")

    # Bit pack the array into uint16_t values, assume width is a multiple of 16
    packed_array = []
    for row in boolean_array:
        packed_row = []
        for i in range(0, len(row), 16):
            packed_byte = 0
            for j in range(16):
                packed_byte |= row[i + j] << (15 - j)
            packed_row.append(packed_byte)
        packed_array.append(packed_row)

    # Print packed array for ATmega328p code as hex
    print("const uint16_t image_array[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {{".format(output_shape[1], output_shape[0]))
    for line in packed_array:
        print("    {" + ", ".join("0x{:04X}".format(byte) for byte in line) + "},")
    print("};")

    return boolean_array

if __name__ == "__main__":
    # Example usage:
    image_to_boolean_array("food.jpg", output_shape=(48, 48), threshold=200, output_file="output_image.bmp")
    image_to_boolean_array("caution.jpg", output_shape=(48, 48), threshold=200, output_file="output_image.bmp")
    image_to_boolean_array("snowflake.jpg", output_shape=(48, 48), threshold=200, output_file="output_image.bmp")
    image_to_boolean_array("ultrasonic.jpg", output_shape=(48, 48), threshold=200, output_file="output_image.bmp")