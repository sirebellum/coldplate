from flask import Flask, request, Response, jsonify, render_template
import struct
from PIL import Image
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

app = Flask(__name__)

# Configuration
MLX90640_RESOLUTION_X = 32
MLX90640_RESOLUTION_Y = 24
DATA_SIZE = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y
IMAGE_DIR = 'images'
os.makedirs(IMAGE_DIR, exist_ok=True)

# Database Configuration
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://username:password@localhost/ir_image_db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

# Database Models
class ImageRecord(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    file_path = db.Column(db.String, unique=True, nullable=False)
    upload_time = db.Column(db.DateTime, default=datetime.utcnow)
    feature_vector = db.Column(db.JSON)

class ProcessedData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    image_id = db.Column(db.Integer, db.ForeignKey('image_record.id'), nullable=False)
    cluster_id = db.Column(db.Integer)
    centroid = db.Column(db.JSON)
    encoded_vector = db.Column(db.JSON)

db.create_all()

# Routes

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

# Upload thermal data (integrated with database)
@app.route('/upload', methods=['GET'])
def upload_thermal_data():
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
@app.route('/process', methods=['POST'])
def process_images():
    subprocess.Popen(["python", "process_images.py"])
    return jsonify({"message": "Processing started"}), 200

# Placeholder for interactive visualization
@app.route('/visualization', methods=['GET'])
def get_visualization():
    try:
        records = ImageRecord.query.all()
        pca_data = [record.feature_vector for record in records if record.feature_vector]

        if not pca_data:
            return jsonify({"message": "No PCA data available."}), 200

        x = [vec[0] for vec in pca_data]
        y = [vec[1] for vec in pca_data]
        z = [vec[2] for vec in pca_data]
        labels = [str(record.id) for record in records]

        scatter = go.Scatter3d(
            x=x, y=y, z=z, mode='markers',
            marker=dict(size=5, color=labels, colorscale='Viridis', opacity=0.8)
        )
        layout = go.Layout(
            title='3D PCA Visualization of IR Images',
            scene=dict(xaxis_title='PC 1', yaxis_title='PC 2', zaxis_title='PC 3')
        )
        fig = go.Figure(data=[scatter], layout=layout)
        graphJSON = json.dumps(fig, cls=PlotlyJSONEncoder)
        return render_template("visualization.html", graphJSON=graphJSON)

    except Exception as e:
        print(f"Error generating visualization: {e}")
        return jsonify({"message": "Failed to generate visualization."}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6969)
