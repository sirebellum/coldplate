import numpy as np
from sklearn.cluster import KMeans
import joblib

# Assuming feature vectors are already extracted by your autoencoder
# Replace this with actual feature extraction code that pulls from your autoencoder
# Example feature vectors from the center of the autoencoder
# Replace with the actual autoencoder feature vector data
def get_feature_vectors():
    # Dummy data for placeholder, replace this with your real feature vectors
    return np.random.rand(1000, 128)  # 1000 samples, 128-dimensional feature vectors

# Number of clusters: 3 (My cat, Roommate's cat, No cat)
num_clusters = 3

# Fetch the feature vectors (from the autoencoder)
features = get_feature_vectors()

# Initialize KMeans with the desired number of clusters
kmeans = KMeans(n_clusters=num_clusters, random_state=42)

# Fit the KMeans algorithm to the feature data
kmeans.fit(features)

# Export the trained KMeans model for later use
joblib.dump(kmeans, 'kmeans_model.pkl')

# Example: Predict clusters for new data (use actual feature vectors here)
new_features = get_feature_vectors()  # Replace with actual new data
predictions = kmeans.predict(new_features)

# Display results (cluster assignments for the new data)
print(predictions)
