import numpy as np
import pandas as pd

def compute_and_print_metrics(y_train, labels):
    unique_labels = np.unique(labels)

    n_samples = len(y_train)
    noise_count = np.sum(labels == -1)
    noise_percent = noise_count / n_samples * 100

    # For non-noise clusters, compute majority-vote accuracy
    clustered_mask = labels != -1
    total_clustered = np.sum(clustered_mask)
    correct = 0
    for lbl in np.unique(labels[clustered_mask]):
        cluster_indices = np.where(labels == lbl)[0]
        true_labels = y_train[cluster_indices]
        # Majority vote:
        counts = np.bincount(true_labels)
        majority_label = np.argmax(counts)
        correct += np.sum(true_labels == majority_label)
    accuracy_in_clusters = (correct / total_clustered * 100) if total_clustered > 0 else 0
    error_in_clusters = 100 - accuracy_in_clusters

    print(f"Total samples: {n_samples}")
    print(f"Number of clusters: {len(unique_labels)-1}") # -1 for noise
    print(f"Noise points: {noise_count} ({noise_percent:.2f}%)")
    print(f"Points in clusters: {total_clustered} ({100 - noise_percent:.2f}%)")
    print(f"Accuracy within clusters (majority-vote): {accuracy_in_clusters:.2f}%")
    print(f"Error rate within clusters: {error_in_clusters:.2f}%")

def compute_and_display_purity(labels, y):
    """
    Compute cluster purity table for DBSCAN results.
    labels: array-like of cluster labels (integer, -1 for noise)
    y: array-like of true labels (integers)
    """
    cluster_info = []
    unique_labels = np.unique(labels)
    cluster_labels = [lbl for lbl in unique_labels if lbl != -1]

    for lbl in sorted(cluster_labels):
        indices = np.where(labels == lbl)[0]
        true_labels = y[indices]
        if len(true_labels) == 0:
            continue
        counts = np.bincount(true_labels)
        maj = np.argmax(counts)
        purity = counts[maj] / len(indices) * 100
        cluster_info.append({
            'Cluster ID': int(lbl),
            'No. points': int(len(indices)),
            'Dominating digit': int(maj),
            'Purity (%)': round(purity, 2)
        })

    df_purity = pd.DataFrame(cluster_info).sort_values('Cluster ID').reset_index(drop=True)

    # Display DataFrame
    print("\nCluster purity table:")
    print(df_purity.to_markdown(index=False))
