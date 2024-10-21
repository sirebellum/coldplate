from flask import Flask, request, Response
import struct
from PIL import Image
import uuid
import base64
import numpy as np
import time

app = Flask(__name__)

# Define the resolution constants
MLX90640_RESOLUTION_X = 32
MLX90640_RESOLUTION_Y = 24
DATA_SIZE = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y

# hearbeat route
@app.route('/heartbeat', methods=['GET'])
def heartbeat():
    return Response("Alive", status=200)

# Get filename
@app.route('/filename', methods=['GET'])
def get_filename():
    filename = str(uuid.uuid4())
    timestamp = time.time()
    filename = f"{filename}_{timestamp}.png"
    return Response(filename, status=200)

# POST route to upload thermal data
@app.route('/upload', methods=['GET'])
def upload_thermal_data():
    # Expecting a base64 encoded string
    try:
        # Get the data from the request
        data = request.args.get('data')
        filename = request.args.get('filename')

        # Decode the base64 string
        data = base64.b64decode(data)

        # Unpack the data
        unpacked_data = struct.unpack(f'{DATA_SIZE}f', data)

        # Reshape the data
        reshaped_data = np.array(unpacked_data).reshape((MLX90640_RESOLUTION_Y, MLX90640_RESOLUTION_X))

        # Normalize the data
        normalized_data = (reshaped_data - reshaped_data.min()) / (reshaped_data.max() - reshaped_data.min())

        # Convert the data to an image
        image = Image.fromarray(np.uint8(normalized_data * 255))

        # Save the image
        image.save(f"images/{filename}")

        return Response("Thermal data received", status=200)
    except Exception as e:
        print(f"Error processing data: {e}")
        return Response("Failed to process data", status=500)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6969)
