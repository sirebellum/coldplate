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
    label_data = []
    for img in images:
        file_path = img.file_path
        label = img.img_class
        image = cv2.imread(file_path, cv2.IMREAD_GRAYSCALE)
        if image is None:
            continue
        normalized_image = image / 255.0
        image_data.append(normalized_image.flatten())
        label_data.append(label)
    return np.array(image_data), np.array(label_data)

def train_kmeans(image_data, labels, num_clusters=NUM_CLUSTERS):
    print("Training KMeans...")

    # Get labels that aren't null
    valid_indices = np.where(labels != None)[0]
    image_data_labeled = image_data[valid_indices]
    labels = labels[valid_indices]

    # Preset centroids based on labels
    centroids = np.zeros((num_clusters, image_data_labeled.shape[1]))
    for i in range(num_clusters):
        cluster_data = image_data_labeled[np.where(labels == i)[0]]
        if len(cluster_data) > 0:
            centroids[i] = np.mean(cluster_data, axis=0)
        else:
            centroids[i] = np.random.rand(image_data_labeled.shape[1])
    kmeans = KMeans(n_clusters=num_clusters, random_state=42, init=centroids)
    print(f"Training KMeans with {len(image_data_labeled)} labeled centroid data points.")

    kmeans.fit(image_data)
    print("KMeans model saved.")
    return kmeans

def main():
    image_data, labels = load_images()
    if len(image_data) == 0:
        print("No images to train on.")
        return

    # Train KMeans
    kmeans = train_kmeans(image_data, labels)

    # Save the models
    torch.save(kmeans, f"kmeans_deploy.pth")

    print("Training complete.")

if __name__ == "__main__":
    main()
