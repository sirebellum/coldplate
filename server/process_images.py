import numpy as np
import cv2
import torch
from sklearn.cluster import KMeans
from cuml.manifold import TSNE  # Import cuML TSNE for GPU acceleration
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
    images = session.query(ImageRecord).all()
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
    print("Applying GPU-accelerated t-SNE...")
    tsne = TSNE(n_components=n_components, random_state=42)
    tsne_data = tsne.fit_transform(image_data)
    return tsne_data

def save_processed_data(labels, vector_data, image_records):
    # Drop current processed data
    session.query(ProcessedData).delete()
    session.commit()

    print("Saving processed data...")
    for i, label in enumerate(labels):
        if np.isnan(vector_data[i]).any():
            continue
        processed = ProcessedData(image_id=image_records[i].id, cluster_id=int(label), encoded_vector=vector_data[i].tolist())
        session.add(processed)
    session.commit()

def training_loop(model, image_data, epochs=10, batch_size=128):
    print("Starting training loop...")
    for epoch in range(epochs):
        batch = image_data[epoch % len(image_data):epoch % len(image_data) + batch_size]
        batch = torch.tensor(batch, dtype=torch.float32)
        batch = batch.reshape(batch.shape[0], 1, IR_Y, IR_X)
        if torch.cuda.is_available():
            batch = batch.to("cuda")
        model.optimizer.zero_grad()
        output = model.forward(batch)
        loss = model.loss_function(output, batch)
        loss.backward()
        model.optimizer.step()
        print(f"Epoch {epoch + 1}: Loss = {loss.item()}")
    print("Training complete!")
    return model

def main():
    image_data, image_records = load_images()
    autoencoder = CNNAutoencoder("ML/model_yamls/visualizer.yaml")
    print(autoencoder)
    if torch.cuda.is_available():
        autoencoder = autoencoder.to("cuda")
    autoencoder = training_loop(autoencoder, image_data, epochs=4200, batch_size=128)

    encoder = autoencoder.get_encoder()
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
