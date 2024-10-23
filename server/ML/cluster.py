import torch
import torch.nn as nn
from sklearn.cluster import KMeans
import cv2
import numpy as np
import os
from torchvision import transforms
from construct_model import CNNAutoencoder
from ir_simulator import MLX90640Simulator

n_clusters = 3
def kmeans_clustering(features, n_clusters):
    # Perform KMeans clustering
    kmeans = KMeans(n_clusters=n_clusters)
    kmeans.fit(features)
    return kmeans

def extract_features(encoder, images):
    # Prepare the images
    images = torch.tensor(images, dtype=torch.float32).unsqueeze(1)

    # Get the features
    with torch.no_grad():
        features = encoder(images).squeeze().cpu().numpy()

    # Reshape for kmeans
    features = features.reshape(features.shape[0], -1)

    return features

import numpy as np

def save_numpy_to_binary(arr, filename="centroids.bin"):
    if not isinstance(arr, np.ndarray):
        raise ValueError("Input must be a NumPy array")
    
    # Save array as a binary file
    arr.astype(np.float32).tofile(filename)

def main():
    # Load the dataset
    simulator = MLX90640Simulator()
    images = simulator.generate_batch(1024 * 10000)

    # Load the model
    model = CNNAutoencoder("model_yamls/default.yaml")

    # Load weights
    model.load_weights("trained_weights.pth")

    # Load the encoder
    encoder = model.encoder

    # Extract features from the dataset
    features = extract_features(encoder, images)

    # Cluster using KMeans
    kmeans = kmeans_clustering(features, n_clusters)

    # Get the cluster centroids
    centroids = kmeans.cluster_centers_

    # Save the centroids to a binary file
    save_numpy_to_binary(centroids)

if __name__ == "__main__":
    main()
