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
from flask_socketio import SocketIO, emit, join_room
import torch
import random

# Temp files
tempfile.tempdir = '/tmp'

app = Flask(__name__)
socketio = SocketIO(app)

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
    img_class = db.Column(db.Integer)

class ProcessedData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    image_id = db.Column(db.Integer, db.ForeignKey('image_record.id'), nullable=False)
    cluster_ids = db.Column(db.JSON)
    encoded_vector = db.Column(db.JSON)

# Ensure tables are created within the application context
with app.app_context():
    db.create_all()

# Thread lock and running flag
process_lock = threading.Lock()
processing_running = False
training_running = False
graphing_running = False
kmeans_training_running = False

# Routes

@app.route('/images/<date>/<filename>', methods=['GET'])
def get_image(date, filename):
    file_path = os.path.join(IMAGE_DIR, date, filename)
    # Load and upsize the image
    image = cv2.imread(file_path, cv2.IMREAD_GRAYSCALE)
    image = cv2.resize(image, (MLX90640_RESOLUTION_X*10, MLX90640_RESOLUTION_Y*10), interpolation=cv2.INTER_NEAREST)
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

# Serve kmeans weights
@app.route('/serve_centroids', methods=['GET'])
def serve_centroids():
    kmeans_file = 'kmeans_deploy.pth'
    kmeans = torch.load(kmeans_file)
    centroids = kmeans.cluster_centers_
    centroids_int = (centroids * 65535).astype(np.uint16)
    centroids_bytes = centroids_int.tobytes()
    centroids_base64 = base64.b64encode(centroids_bytes).decode('utf-8')
    return Response(centroids_base64, status=200)

@app.route('/centroids_count', methods=['GET'])
def centroids_count():
    kmeans_file = 'kmeans_deploy.pth'
    kmeans = torch.load(kmeans_file)
    return Response(str(kmeans.n_clusters), status=200)

# Upload PNG image
@app.route('/upload_png', methods=['POST'])
def upload_png():
    try:
        # Get the data from the request
        data = request.json
        image = data['image']

        # Decode the base64 string
        image = base64.b64decode(image)

        # Generate a unique filename
        filename = f"{uuid.uuid4()}.png"

        # Save to the directory named simulated
        simulated_dir = os.path.join(IMAGE_DIR, 'simulated')
        os.makedirs(simulated_dir, exist_ok=True)

        # Save the image
        file_path = os.path.join(simulated_dir, filename)
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
@app.route('/upload', methods=['POST'])
def upload_thermal_data():
    try:
        # Get the data from the request
        data = request.json
        data = data['data']

        # Decode the base64 string
        data = base64.b64decode(data)

        # Unpack the data
        unpacked_data = struct.unpack(f'{DATA_SIZE}H', data)

        # Reshape the data
        reshaped_data = np.array(unpacked_data).reshape((MLX90640_RESOLUTION_Y, MLX90640_RESOLUTION_X))

        # Flip the data vertically
        reshaped_data = np.flipud(reshaped_data)

        # Normalize the data
        normalized_data = (reshaped_data - reshaped_data.min()) / (reshaped_data.max() - reshaped_data.min())

        # Convert the data to an image
        image = Image.fromarray(np.uint8(normalized_data * 255))

        # Generate a unique filename
        filename = f"{uuid.uuid4()}.png"
        
        # Get the date for the directory
        date = datetime.now().strftime("%Y-%m-%d")
        date_dir = os.path.join(IMAGE_DIR, date)
        os.makedirs(date_dir, exist_ok=True)

        # Save the image
        file_path = os.path.join(date_dir, filename)
        image.save(file_path)

        # Save reference in the database
        new_image = ImageRecord(file_path=file_path)
        db.session.add(new_image)
        db.session.commit()

        return Response("Thermal data received", status=200)
    except Exception as e:
        print(f"Error processing data: {e}")
        return Response("Failed to process data", status=500)

def run_training():
    global training_running
    if training_running:
        print("Training already running.")
        return
    with app.app_context():
        try:
            training_running = True
            subprocess.run(["python", "train_images.py"])
        finally:
            with process_lock:
                training_running = False

def run_processing():
    global processing_running
    if processing_running:
        print("Processing already running.")
        return
    while training_running:
        print("Waiting for training to complete...")
        time.sleep(5)
    with app.app_context():
        try:
            processing_running = True
            subprocess.run(["python", "process_images.py"])
        finally:
            with process_lock:
                processing_running = False

def run_kmeans_training():
    global kmeans_training_running
    if kmeans_training_running:
        print("KMeans training already running.")
        return
    with app.app_context():
        try:
            kmeans_training_running = True
            subprocess.run(["python", "train_kmeans.py"])
        finally:
            with process_lock:
                kmeans_training_running = False

# Endpoint for Visualization
@app.route('/visualization', methods=['GET'])
def get_visualization():
    return render_template("visualization.html")

# High contrast colors
colors = ['red', 'blue', 'green', 'purple', 'orange', 'yellow', 'pink', 'cyan', 'magenta', 'lime', 'teal', 'lavender', 'brown', 'beige', 'maroon', 'mint', 'olive', 'apricot', 'navy', 'grey', 'white', 'black']
colors_map = {i: color for i, color in enumerate(colors)}
def get_graph_json(x, y, labels, image_urls):
    # Get the cluster ids
    cluster_ids = labels[0].keys()

    # Seperate out the labels for each cluster
    labels_color = {}
    for cluster_id in cluster_ids:
        labels_num = [labels[i][cluster_id] for i in range(len(labels))]
        labels_color[cluster_id] = [colors_map[label] for label in labels_num]

    # Create the scatter plots
    scatters = []
    layout = go.Layout(
        title=f't-SNE Visualization',
        xaxis=dict(title='PC 1'),
        yaxis=dict(title='PC 2'),
        paper_bgcolor='rgb(30, 30, 30)',
        plot_bgcolor='rgb(30, 30, 30)',   # Set the plot area background color to dark gray
        font=dict(color='white'),  # Set the font color to white for better contrast
        legend=dict(
            font=dict(color='white'),
            bgcolor='rgb(128, 128, 128)'
        )
    )
    for cluster_id in cluster_ids:
        if cluster_id != 'deploy':
            continue
        scatter = go.Scatter(
            x=x, y=y,
            mode='markers',
            marker=dict(
                color=labels_color[cluster_id],
                size=5,
                opacity=0.8
            ),
            customdata=image_urls,
            name=f'KMeans {cluster_id}',
        )
        scatters.append(scatter)
    fig = go.Figure(data=scatters, layout=layout)
    graphJSON = json.dumps(fig, cls=PlotlyJSONEncoder)

    return graphJSON

# socketio function to run in the background looping through the data
# to emit graph points to the client
def emit_graph():
    global graphing_running
    if graphing_running:
        print("Graphing already running.")
        return
    while processing_running:
        print("Waiting for processing to complete...")
        time.sleep(5)
    with app.app_context():
        graphing_running = True
        records = ProcessedData.query.all()
        if len(records) == 0:
            print("No data to graph.")
            graphing_running = False
            return
        sampled_items = records[-10000:]
        x = [data.encoded_vector[0] for data in sampled_items]
        y = [data.encoded_vector[1] for data in sampled_items]
        labels = [data.cluster_ids for data in sampled_items]
        image_ids = [data.image_id for data in sampled_items]
        image_records = ImageRecord.query.filter(ImageRecord.id.in_(image_ids)).all()
        image_urls = [f"/{image.file_path}" for image in image_records]
        graphJSON = get_graph_json(x, y, labels, image_urls)
        graphing_running = False
        print("Graphing complete.")
        socketio.emit('update_graph', graphJSON)

# Handle Update Graph
@socketio.on('update_graph')
def update_graph():
    socketio.start_background_task(emit_graph)

# Handle Process Button Event
@socketio.on('process_images')
def process_images():
    print("Process images event received.")
    socketio.start_background_task(run_processing)
    socketio.start_background_task(emit_graph)

# Handle Train Button Event
@socketio.on('train_model')
def train_model():
    print("Train model event received.")
    socketio.start_background_task(run_training)
    socketio.start_background_task(run_processing)
    socketio.start_background_task(emit_graph)

# Handle Train KMeans Button Event
@socketio.on('train_kmeans')
def train_kmeans():
    print("Train KMeans event received.")
    socketio.start_background_task(run_kmeans_training)

# Handle Status
@socketio.on('status')
def status():
    emit('status_response', {
        'processing_running': processing_running,
        'training_running': training_running,
        'graphing_running': graphing_running,
        'kmeans_training_running': kmeans_training_running
    })

@socketio.on('label_multiple_data')
def label_multiple_data(data):
    print(f"Label multiple data received: {data}")
    selected_points = data['selectedPoints']
    class_label = data['class']

    for point in selected_points:
        x = point['x']
        y = point['y']
        image_url = point['imageUrl']

        # Example logic to extract image ID and save to database
        image_id = extract_image_id_from_url(image_url)
        if not image_id:
            print(f"Image ID not found for {image_url}")
            continue
        
        # Update the image class
        image_record = ImageRecord.query.get(image_id)
        image_record.img_class = class_label
        db.session.add(image_record)

    db.session.commit()
    print(f"Applied class {class_label} to {len(selected_points)} points.")

def extract_image_id_from_url(image_url):
    # Implement extraction of image ID from the URL, depending on how your image paths are structured
    file_name = image_url.split('/')[-1]
    image_record = ImageRecord.query.filter_by(file_path=file_name).first()
    return image_record.id if image_record else None

# Event handler for requested db info
@socketio.on('get_db_info')
def get_db_info():
    num_images = ImageRecord.query.count()
    num_processed = ProcessedData.query.count()
    emit('db_info', {
        'imageCount': num_images,
        'processedCount': num_processed
    })

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=6969)
