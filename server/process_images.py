import numpy as np
import cv2
from sklearn.cluster import KMeans
from sklearn.decomposition import PCA
from models import db, ImageRecord, ProcessedData

IMAGE_DIR = 'images'
PCA_COMPONENTS = 3
NUM_CLUSTERS = 5

def load_images():
    images = ImageRecord.query.all()
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
    kmeans = KMeans(n_clusters=num_clusters, random_state=42)
    kmeans.fit(image_data)
    return kmeans.labels_, kmeans.cluster_centers_

def apply_pca(image_data, n_components=PCA_COMPONENTS):
    pca = PCA(n_components=n_components)
    pca_data = pca.fit_transform(image_data)
    return pca_data

def save_cluster_data(labels, image_records):
    for idx, img in enumerate(image_records):
        processed = ProcessedData(image_id=img.id, cluster_id=int(labels[idx]))
        db.session.add(processed)
    db.session.commit()

def save_pca_data(pca_data, image_records):
    for idx, img in enumerate(image_records):
        img.feature_vector = pca_data[idx].tolist()
        db.session.commit()

def main():
    image_data, image_records = load_images()
    labels, _ = perform_clustering(image_data)
    save_cluster_data(labels, image_records)
    pca_data = apply_pca(image_data)
    save_pca_data(pca_data, image_records)
    print("Processing complete. Data saved.")

if __name__ == "__main__":
    main()
