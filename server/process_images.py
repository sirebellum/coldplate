import numpy as np
import cv2
import torch
from models import Session, ImageRecord, ProcessedData
from ML.construct_model import CNNAutoencoder
from MulticoreTSNE import MulticoreTSNE as TSNE
import os


IR_X = 32
IR_Y = 24
TSNE_COMPONENTS = 2  # Number of components for t-SNE

session = Session()

def load_images(num_images=None):
    images = session.query(ImageRecord).all()
    if num_images is not None:
        images = images[:num_images]
    image_data = []
    image_records = []
    for img in images:
        file_path = img.file_path
        image = cv2.imread(file_path, cv2.IMREAD_GRAYSCALE)
        if image is None:
            continue
        normalized_image = image / 255.0
        image_data.append(normalized_image.flatten())
        image_records.append(img)
    return np.array(image_data), image_records

def encode_images(image_data, encoder):
    image_data = torch.tensor(image_data, dtype=torch.float32)
    image_data = image_data.reshape(image_data.shape[0], 1, IR_Y, IR_X)
    batch_size = 128
    vector_data = []
    for i in range(0, len(image_data), batch_size):
        batch = image_data[i:i + batch_size]
        if torch.cuda.is_available():
            batch = batch.to("cuda")
        vector = encoder(batch).cpu().detach().numpy()
        vector_data.append(vector)
    return np.concatenate(vector_data, axis=0)

def load_kmeans():
    if not os.path.exists("kmeans_model.pth"):
        print("KMeans model not found.")
        return None
    kmeans = torch.load("kmeans_model.pth")
    return kmeans

def perform_tsne(vector_data):
    print("Performing t-SNE...")
    tsne = TSNE(n_components=TSNE_COMPONENTS, n_jobs=16)
    tsne_data = tsne.fit_transform(vector_data)
    return tsne_data

def save_processed_data(labels, tsne_data, image_records):
    print("Saving processed data...")

    session.query(ProcessedData).delete()

    for i, label in enumerate(labels):
        if np.isnan(tsne_data[i]).any():
            continue
        processed = ProcessedData(image_id=image_records[i].id, cluster_id=int(label), encoded_vector=tsne_data[i].tolist())
        session.add(processed)
    session.commit()

def main():
    image_data, image_records = load_images(100000)
    if len(image_data) == 0:
        print("No images to process.")
        return

    # Load the encoder weights
    encoder = CNNAutoencoder("ML/model_yamls/visualizer.yaml").get_encoder()
    if not os.path.exists("encoder_weights.pth"):
        print("Encoder weights not found.")
        return
    try:
        encoder.load_state_dict(torch.load("encoder_weights.pth"))
    except RuntimeError:
        print("Error loading encoder weights.")
        return
    if torch.cuda.is_available():
        encoder = encoder.to("cuda")

    vector_data = encode_images(image_data, encoder)
    kmeans = load_kmeans()
    if kmeans is None:
        print("KMeans model not found.")
        return

    # Predict clusters
    labels = kmeans.predict(vector_data)
    tsne_data = perform_tsne(vector_data)

    save_processed_data(labels, tsne_data, image_records)
    print("Processing complete. Data saved.")

if __name__ == "__main__":
    main()
