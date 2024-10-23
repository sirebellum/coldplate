import numpy as np
import cv2
import torch
from sklearn.cluster import KMeans
from models import Session, ImageRecord
from ML.construct_model import CNNAutoencoder

IR_X = 32
IR_Y = 24
NUM_CLUSTERS = 4  # Number of clusters for KMeans

session = Session()

def load_images():
    images = session.query(ImageRecord).all()
    image_data = []
    for img in images:
        file_path = img.file_path
        image = cv2.imread(file_path, cv2.IMREAD_GRAYSCALE)
        if image is None:
            continue
        normalized_image = image / 255.0
        image_data.append(normalized_image.flatten())
    return np.array(image_data)

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

def train_kmeans(image_data, num_clusters=NUM_CLUSTERS):
    print("Training KMeans...")
    kmeans = KMeans(n_clusters=num_clusters, random_state=42)
    kmeans.fit(image_data)
    torch.save(kmeans, "kmeans_model.pth")
    print("KMeans model saved.")

def main():
    image_data = load_images()
    if len(image_data) == 0:
        print("No images to train on.")
        return

    # Load the encoder weights
    encoder = CNNAutoencoder("ML/model_yamls/visualizer.yaml").get_encoder()
    encoder.load_state_dict(torch.load("encoder_weights.pth"))
    if torch.cuda.is_available():
        encoder = encoder.to("cuda")

    vector_data = encode_images(image_data, encoder)

    # Train and save KMeans and t-SNE
    train_kmeans(vector_data)

    print("Training complete.")

if __name__ == "__main__":
    main()
