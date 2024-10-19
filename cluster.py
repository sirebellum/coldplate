from ir_simulator import MLX90640Simulator
import numpy as np
from sklearn.cluster import KMeans
from metrics import adjusted_rand_index, normalized_mutual_info, fowlkes_mallows_index

n_clusters = 5

def calculate_max_scaling_factor_unsigned(centroids):
    # Find the maximum value in the centroids
    max_value = np.max(centroids)
    
    # Start with a high scaling factor and adjust down if necessary
    scaling_factor = 1
    max_uint_value = 2**32 - 1
    
    # Increase scaling factor to maximize precision until just before overflow
    while True:
        next_scaling_factor = scaling_factor * 10
        max_scaled_value = max_value * next_scaling_factor
        
        if max_scaled_value > max_uint_value:
            break
        
        scaling_factor = next_scaling_factor
    
    # Calculate the effective precision achieved
    effective_precision = 1 / scaling_factor
    
    return scaling_factor, effective_precision

def main():
    # Load the dataset
    simulator = MLX90640Simulator()
    images_labeled, labels = simulator.generate_batch(1024)
    images, _ = simulator.generate_batch(1024 * 1024)

    # Flatten the images
    images = images.reshape(images.shape[0], -1)
    images_labeled = images_labeled.reshape(images_labeled.shape[0], -1)

    # Get initial centroids based on the labeled images
    unique_labels = np.unique(labels)
    initial_centroids = np.array([images_labeled[labels == label].mean(axis=0) for label in unique_labels])
    
    # Perform KMeans clustering
    kmeans = KMeans(n_clusters=n_clusters, init=initial_centroids, n_init=1)
    kmeans.fit(images)

    # Check accuracy
    predictions = kmeans.predict(images_labeled)

    # Get the cluster centroids
    centroids = kmeans.cluster_centers_

    # Scale the centroids
    scaling_factor, effective_precision = calculate_max_scaling_factor_unsigned(centroids)
    print("Scaling factor:", scaling_factor)
    print("Effective precision:", effective_precision)

    # Scale the centroids
    scaled_centroids = centroids * scaling_factor

    # Round the scaled centroids
    rounded_centroids = np.round(scaled_centroids).astype(int)

    # Print the rounded centroids[n_clusters][768] as a c++ array in hex
    # Print n_clusters items per line
    print("const uint32_t centroids[KMEANS_CENTROIDS][KMEANS_DIMENSIONALITY] = {")
    for i in range(n_clusters):
        print("{", end="")
        for j in range(768):
            print("0x{:08X}".format(rounded_centroids[i][j]), end="")
            if j < 767:
                print(", ", end="")
            if j % 8 == 7:
                print("")
        print("},")
    print("};")

    # Print detailed accuracy information
    print("Adjusted Rand Index:", adjusted_rand_index(labels, predictions))
    print("Normalized Mutual Information:", normalized_mutual_info(labels, predictions))
    print("Fowlkes-Mallows Index:", fowlkes_mallows_index(labels, predictions))

if __name__ == "__main__":
    main()
