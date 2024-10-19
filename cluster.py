from sklearn.cluster import KMeans
from sklearn.metrics import mutual_info_score
import numpy as np
from ir_simulator import MLX90640Simulator
from scipy.special import comb

n_clusters = 5
def calculate_max_scaling_factor_unsigned(centroids):
    # Since centroids are between 0 and 1, the max_value is <= 1
    max_value = 1.0
    
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

def adjusted_rand_index(true_labels, pred_labels):
    n = len(true_labels)
    contingency_matrix = {}
    
    # Create contingency table
    for true, pred in zip(true_labels, pred_labels):
        if (true, pred) in contingency_matrix:
            contingency_matrix[(true, pred)] += 1
        else:
            contingency_matrix[(true, pred)] = 1

    sum_comb_c = sum(comb(n_ij, 2) for n_ij in contingency_matrix.values())
    sum_comb_true = sum(comb(sum(true_labels == k), 2) for k in set(true_labels))
    sum_comb_pred = sum(comb(sum(pred_labels == k), 2) for k in set(pred_labels))
    
    index = sum_comb_c
    expected_index = (sum_comb_true * sum_comb_pred) / comb(n, 2)
    max_index = 0.5 * (sum_comb_true + sum_comb_pred)
    
    if max_index == expected_index:
        return 1.0
    
    return (index - expected_index) / (max_index - expected_index)

def normalized_mutual_info(true_labels, pred_labels):
    mutual_info = mutual_info_score(true_labels, pred_labels)
    h_true = -np.sum(np.bincount(true_labels) / len(true_labels) * np.log(np.bincount(true_labels) / len(true_labels)))
    h_pred = -np.sum(np.bincount(pred_labels) / len(pred_labels) * np.log(np.bincount(pred_labels) / len(pred_labels)))
    
    return 2 * mutual_info / (h_true + h_pred)

def fowlkes_mallows_index(true_labels, pred_labels):
    # Create a confusion matrix for pairs
    tp_fp = sum(comb(sum(pred_labels == k), 2) for k in set(pred_labels))
    tp_fn = sum(comb(sum(true_labels == k), 2) for k in set(true_labels))
    
    # True positive pairs
    tp = 0
    n = len(true_labels)
    for i in range(n):
        for j in range(i + 1, n):
            if true_labels[i] == true_labels[j] and pred_labels[i] == pred_labels[j]:
                tp += 1
                
    # Precision and Recall calculations
    precision = tp / tp_fp if tp_fp != 0 else 0
    recall = tp / tp_fn if tp_fn != 0 else 0
    
    # FMI is the geometric mean of precision and recall
    return np.sqrt(precision * recall)

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
