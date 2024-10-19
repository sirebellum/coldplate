from sklearn.metrics import normalized_mutual_info_score
from scipy.special import comb
from collections import Counter

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
    # Contingency table
    contingency_matrix = np.zeros((len(set(true_labels)), len(set(pred_labels))), dtype=int)
    label_mapping_true = {label: idx for idx, label in enumerate(set(true_labels))}
    label_mapping_pred = {label: idx for idx, label in enumerate(set(pred_labels))}
    
    for true, pred in zip(true_labels, pred_labels):
        contingency_matrix[label_mapping_true[true], label_mapping_pred[pred]] += 1

    # Sum of combinations of counts
    sum_comb_c = sum(comb(n_ij, 2) for n_ij in contingency_matrix.flatten() if n_ij >= 2)
    sum_comb_true = sum(comb(n, 2) for n in Counter(true_labels).values() if n >= 2)
    sum_comb_pred = sum(comb(n, 2) for n in Counter(pred_labels).values() if n >= 2)
    
    total_pairs = comb(len(true_labels), 2)
    expected_index = (sum_comb_true * sum_comb_pred) / total_pairs
    max_index = 0.5 * (sum_comb_true + sum_comb_pred)
    
    if max_index == expected_index:
        return 1.0
    
    return (sum_comb_c - expected_index) / (max_index - expected_index)

def normalized_mutual_info(true_labels, pred_labels):
    # Convert to numpy arrays to handle negative values safely
    true_labels = np.array(true_labels)
    pred_labels = np.array(pred_labels)
    
    # Ensure no negative values are present by filtering them out together
    mask = (true_labels >= 0) & (pred_labels >= 0)
    true_labels = true_labels[mask]
    pred_labels = pred_labels[mask]
    
    # Check if the lengths match after filtering
    if len(true_labels) != len(pred_labels):
        raise ValueError("The length of true_labels and pred_labels must match after filtering out negatives.")
    
    # Use sklearn's implementation of NMI
    return normalized_mutual_info_score(true_labels, pred_labels)

def fowlkes_mallows_index(true_labels, pred_labels):
    # Convert to numpy arrays
    true_labels = np.array(true_labels)
    pred_labels = np.array(pred_labels)
    
    # Ensure arrays are the same length
    min_length = min(len(true_labels), len(pred_labels))
    true_labels = true_labels[:min_length]
    pred_labels = pred_labels[:min_length]
    
    # True positive pairs (TP)
    tp = 0
    n = len(true_labels)
    for i in range(n):
        for j in range(i + 1, n):
            if true_labels[i] == true_labels[j] and pred_labels[i] == pred_labels[j]:
                tp += 1
    
    # Count of pairs in predicted clusters (TP + FP)
    tp_fp = sum(comb(np.sum(pred_labels == k), 2) for k in set(pred_labels))
    
    # Count of pairs in true clusters (TP + FN)
    tp_fn = sum(comb(np.sum(true_labels == k), 2) for k in set(true_labels))
    
    # Precision and Recall calculations
    precision = tp / tp_fp if tp_fp != 0 else 0
    recall = tp / tp_fn if tp_fn != 0 else 0
    
    # FMI is the geometric mean of precision and recall
    return np.sqrt(precision * recall)