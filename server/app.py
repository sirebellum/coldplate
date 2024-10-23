from flask import Flask, request, Response, jsonify, render_template, send_from_directory
import struct
from PIL import Image
import cv2
import uuid
import base64
import numpy as np
import time
import os
import subprocess
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy import func
from datetime import datetime
import plotly.graph_objs as go
from plotly.utils import PlotlyJSONEncoder
import json
import tempfile
import threading

# Temp files
tempfile.tempdir = '/tmp'

app = Flask(__name__)

# Configuration
MLX90640_RESOLUTION_X = 32
MLX90640_RESOLUTION_Y = 24
DATA_SIZE = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y
IMAGE_DIR = 'images'
os.makedirs(IMAGE_DIR, exist_ok=True)

# Database Configuration
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://ir_user:securepassword@192.168.69.9/ir_image_db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

# Database Models
class ImageRecord(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    file_path = db.Column(db.String, unique=True, nullable=False)
    upload_time = db.Column(db.DateTime, default=datetime.utcnow)

class ProcessedData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    image_id = db.Column(db.Integer, db.ForeignKey('image_record.id'), nullable=False)
    cluster_id = db.Column(db.Integer)
    centroid = db.Column(db.JSON)
    encoded_vector = db.Column(db.JSON)

# Ensure tables are created within the application context
with app.app_context():
    db.create_all()

# Thread lock and running flag
process_lock = threading.Lock()
processing_running = False

# Routes

@app.route('/images/<filename>', methods=['GET'])
def get_image(filename):
    # Load and upsize the image
    image = cv2.imread(os.path.join(IMAGE_DIR, filename), cv2.IMREAD_GRAYSCALE)
    image = cv2.resize(image, (MLX90640_RESOLUTION_Y*10, MLX90640_RESOLUTION_X*10), interpolation=cv2.INTER_NEAREST)
    temp_file = tempfile.NamedTemporaryFile(delete=False, suffix='.png')
    cv2.imwrite(temp_file.name, image)
    return send_from_directory(tempfile.gettempdir(), os.path.basename(temp_file.name))

@app.route('/', methods=['GET'])
def home():
    return render_template("index.html")


# Heartbeat route
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

# Upload PNG image
@app.route('/upload_png', methods=['POST'])
def upload_png():
    try:
        # Get the data from the request
        data = request.json
        filename = data['filename']
        image = data['image']

        # Decode the base64 string
        image = base64.b64decode(image)

        # Save the image
        file_path = os.path.join(IMAGE_DIR, filename)
        with open(file_path, 'wb') as f:
            f.write(image)

        # Save reference in the database
        new_image = ImageRecord(file_path=file_path)
        db.session.add(new_image)
        db.session.commit()

        return Response("PNG image received", status=200)
    except Exception as e:
        print(f"Error processing data: {e}")
        return Response("Failed to process data", status=500)

# Upload thermal data (integrated with database)
@app.route('/upload', methods=['GET'])
def upload_thermal_data():
    try:
        # Get the data from the request
        data = request.args.get('data')
        filename = request.args.get('filename')

        # Decode the base64 string
        data = base64.b64decode(data)
        print(len(data))

        # Unpack the data
        unpacked_data = struct.unpack(f'{DATA_SIZE}f', data)

        # Reshape the data
        reshaped_data = np.array(unpacked_data).reshape((MLX90640_RESOLUTION_Y, MLX90640_RESOLUTION_X))

        # Normalize the data
        normalized_data = (reshaped_data - reshaped_data.min()) / (reshaped_data.max() - reshaped_data.min())

        # Convert the data to an image
        image = Image.fromarray(np.uint8(normalized_data * 255))

        # Save the image
        file_path = os.path.join(IMAGE_DIR, filename)
        image.save(file_path)

        # Save reference in the database
        new_image = ImageRecord(file_path=file_path)
        db.session.add(new_image)
        db.session.commit()

        return Response("Thermal data received", status=200)
    except Exception as e:
        print(f"Error processing data: {e}")
        return Response("Failed to process data", status=500)

# Trigger batch processing via endpoint
@app.route('/process', methods=['GET'])
def process_images():
    global processing_running
    with process_lock:
        if processing_running:
            return jsonify({"message": "Processing is already running"}), 400

        # Set the flag to indicate that processing is running
        processing_running = True

    # Start the processing in a separate thread
    def run_processing():
        try:
            subprocess.run(["python", "process_images.py"])
        finally:
            # Reset the flag once processing is complete
            global processing_running
            with process_lock:
                processing_running = False

    # Launch the thread
    threading.Thread(target=run_processing).start()
    return jsonify({"message": "Processing started"}), 200

# Placeholder for interactive visualization
@app.route('/visualization', methods=['GET'])
def get_visualization():
    try:
        processed_data = ProcessedData.query.all()
        vector_data = []
        labels = []
        image_urls = []
        for processed in processed_data:
            if processed.encoded_vector:
                record = ImageRecord.query.filter_by(id=processed.image_id).first()
                vector_data.append(processed.encoded_vector)
                labels.append(processed.cluster_id)
                image_urls.append(record.file_path)
            
        # high contrast colors
        colors = ['red', 'blue', 'green', 'purple', 'orange', 'yellow', 'pink', 'cyan', 'magenta', 'lime', 'teal', 'lavender', 'brown', 'beige', 'maroon', 'mint', 'olive', 'apricot', 'navy', 'grey', 'white', 'black']
        colors_map = {i: color for i, color in enumerate(colors)}
        labels_color = [colors_map[label] for label in labels]

        if not vector_data:
            return jsonify({"message": "No Vector data available."}), 200

        x = [vec[0] for vec in vector_data]
        y = [vec[1] for vec in vector_data]

        scatter = go.Scatter(
            x=x, y=y,
            mode='markers',
            marker=dict(
                color=labels_color,
                size=5,
                opacity=0.8
            ),
            customdata=image_urls,
            # hoverinfo='none'  # Disable default hover info
        )
        layout = go.Layout(
            title='2D Cluster Visualization of IR Images',
            scene=dict(xaxis_title='PC 1', yaxis_title='PC 2')
        )
        fig = go.Figure(data=[scatter], layout=layout)
        graphJSON = json.dumps(fig, cls=PlotlyJSONEncoder)
        return render_template("visualization.html", graphJSON=graphJSON)

    except Exception as e:
        print(f"Error generating visualization: {e}")
        return jsonify({"message": "Failed to generate visualization."}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6969)
