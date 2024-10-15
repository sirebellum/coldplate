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
    print("const bool image_array[SPLASH_HEIGHT][SPLASH_WIDTH] PROGMEM = {{".format(output_shape[1], output_shape[0]))
    for line in formatted_array:
        print("    {" + ", ".join(line) + "},")
    print("};")

    # # Convert boolean array to list of indices of True values
    # indices = []
    # for i, row in enumerate(boolean_array):
    #     for j, pixel in enumerate(row):
    #         if pixel:
    #             indices.append((i, j))

    # # Print the indices as a formatted array for use in ATmega328p code, multiple indices per line
    # print("const uint8_t image_indices[{}][2] PROGMEM = {{".format(len(indices)))
    # for i in range(0, len(indices), 8):
    #     print("    {" + "}, {".join([f"{y}, {x}" for x, y in indices[i:i+8]]) + "},")
    # print("};")

    return boolean_array

if __name__ == "__main__":
    # Example usage:
    image_to_boolean_array("caution.jpg", output_shape=(64, 64), threshold=200, output_file="output_image.bmp")
