import numpy as np
import cv2
import torch
from sklearn.cluster import KMeans
from MulticoreTSNE import MulticoreTSNE as TSNE  # Updated import
from models import Session, ImageRecord, ProcessedData
from ML.construct_model import CNNAutoencoder

IMAGE_DIR = 'images'
TSNE_COMPONENTS = 2  # Number of components for t-SNE
NUM_CLUSTERS = 4  # Number of clusters for KMeans

IR_X = 32
IR_Y = 24

# Create a new session
session = Session()

def load_images():
    # Only get unprocessed images
    processed_ids = [p.image_id for p in session.query(ProcessedData).all()]
    images = session.query(ImageRecord).filter(~ImageRecord.id.in_(processed_ids)).all()
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

def perform_clustering(image_data, num_clusters=NUM_CLUSTERS):
    print("Performing clustering...")
    kmeans = KMeans(n_clusters=num_clusters, random_state=42)
    kmeans.fit(image_data)
    return kmeans.labels_, kmeans.cluster_centers_

def apply_tsne(image_data, n_components=TSNE_COMPONENTS):
    print("Applying t-SNE...")
    tsne = TSNE(n_components=n_components, random_state=42, n_jobs=16)  # Added n_jobs for multicore
    tsne_data = tsne.fit_transform(image_data)
    return tsne_data

def save_processed_data(labels, vector_data, image_records):
    print("Saving processed data...")
    for i, label in enumerate(labels):
        if np.isnan(vector_data[i]).any():
            continue
        processed = ProcessedData(image_id=image_records[i].id, cluster_id=int(label), encoded_vector=vector_data[i].tolist())
        session.add(processed)
    session.commit()

def main():
    image_data, image_records = load_images()
    if len(image_data) == 0:
        print("No new images to process.")
        return

    # Load the encoder weights
    encoder = CNNAutoencoder("ML/model_yamls/visualizer.yaml").get_encoder()
    encoder.load_state_dict(torch.load("encoder_weights.pth"))
    print("Encoder weights loaded.")

    if torch.cuda.is_available():
        encoder = encoder.to("cuda")

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
    vector_data = np.concatenate(vector_data, axis=0)

    labels, _ = perform_clustering(vector_data)
    tsne_data = apply_tsne(vector_data)
    save_processed_data(labels, tsne_data, image_records)
    print("Processing complete. Data saved.")

if __name__ == "__main__":
    main()
