from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt
import seaborn as sns
import tensorflow as tf
from sklearn.neighbors import NearestNeighbors

class Dbscan:
    def __init__(self, eps, min_samples, verbose=False):
        self.eps = eps
        self.min_samples = min_samples
        self.verbose = verbose
        self.labels = None  # Array to store cluster labels (-1 for noise)
        self.visited = set() # Set of indices of visited points

    def fit(self, X):
        """
        Perform DBSCAN clustering on data X.
        X: numpy array of shape (n_samples, n_features)
        Returns: labels array of shape (n_samples)
        """
        n_samples = X.shape[0]
        # Initialize labels to -1 (noise/unassigned)
        labels = -1 * np.ones(n_samples, dtype=int)
        visited = np.zeros(n_samples, dtype=bool)
        cluster_id = 0

        # Precompute neighbors using radius_neighbors
        nbrs = NearestNeighbors(radius=self.eps).fit(X)
        all_neighbors = nbrs.radius_neighbors(X, return_distance=False)

        for i in range(n_samples):
            if visited[i]:
                continue
            visited[i] = True
            neighbors = all_neighbors[i]
            # Check if core point
            if len(neighbors) < self.min_samples:
                # Mark as noise (remains -1)
                if self.verbose:
                    print(f"Point {i} is noise (only {len(neighbors)} neighbors)")
                continue
            # Otherwise, start a new cluster
            if self.verbose:
                print(f"Creating cluster {cluster_id} starting at point {i}")
            labels[i] = cluster_id
            # Use a list as queue for expansion
            # Exclude the point itself if present among neighbors
            seed_queue = list(neighbors[neighbors != i])
            # Expand cluster
            while seed_queue:
                j = seed_queue.pop(0)
                if not visited[j]:
                    visited[j] = True
                    neighbors_j = all_neighbors[j]
                    if len(neighbors_j) >= self.min_samples:
                        # If core point, add its neighbors to the queue
                        for nb in neighbors_j:
                            if nb not in seed_queue:
                                seed_queue.append(nb)
                # Assign to cluster if not yet assigned
                if labels[j] == -1:
                    labels[j] = cluster_id
            cluster_id += 1

        self.labels = labels
        return labels

def load_mnist():
    mnistDataSet = tf.keras.datasets.mnist

    (x_train, y_train), (x_test, y_test) = mnistDataSet.load_data()
    x_train, x_test = x_train / 255.0, x_test / 255.0
    x_train = x_train.reshape((-1, x_train.shape[1]*x_train.shape[2]))
    x_test = x_test.reshape((-1, x_test.shape[1]*x_test.shape[2]))

    x_train = x_train[:5000]
    y_train = y_train[:5000]

    return x_train, y_train

def reduce_dimensions(x_train):
    # We need to reduce the number of dimensions in order for DBSCAN to work correctly
    # DBSCAN has a curse of dimensionality (and 784 is a lot of dimensions)
    print("Applying PCA...")
    pca = PCA(n_components=50) # reduce dimensions from 784 to 50
    pca_res_50 = pca.fit_transform(x_train)

    print("Applying T-SNE...")
    tsne = TSNE(n_components = 2, random_state=0) # reduce dimensions from 50 to 2
    tsne_res = tsne.fit_transform(pca_res_50)

    print("Dimensions reduced")
    return tsne_res

if __name__ == '__main__':

    x_train, y_train = load_mnist()
    tsne_train = reduce_dimensions(x_train)

    print("Data plot with real labels:")
    plt.figure(figsize=(16,10))
    sns.scatterplot(x = tsne_train[:,0], y = tsne_train[:,1], hue = y_train, legend = 'full') #palette = sns.hls_palette(10),

    eps = 2.4
    min_samples = 7
    dbscan = Dbscan(eps, min_samples)
    labels = dbscan.fit(tsne_train)

    # plot clusters
    plt.figure(figsize=(8,6))
    unique_labels = np.unique(labels)
    colors = plt.cm.get_cmap('tab20', len(unique_labels))
    for lbl in unique_labels:
        if lbl == -1:
            color = 'k'
            marker = 'x'
            size = 15
            label_name = "Noise"
        else:
            color = colors(lbl)
            marker = 'o'
            size = 10
            label_name = f"Cluster {lbl}"
        pts = tsne_train[labels == lbl]
        plt.scatter(pts[:,0], pts[:,1], c=[color], s=size, marker=marker, label=label_name)
    plt.legend(markerscale=2, bbox_to_anchor=(1.05,1), loc='upper left', fontsize='small')
    plt.title(f"DBSCAN Clustering (eps={eps}, min_samples={min_samples})")
    plt.show()

    # Compute metrics
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
    print(f"Noise points: {noise_count} ({noise_percent:.2f}%)")
    print(f"Points in clusters: {total_clustered} ({100 - noise_percent:.2f}%)")
    print(f"Accuracy within clusters (majority-vote): {accuracy_in_clusters:.2f}%")
    print(f"Error rate within clusters: {error_in_clusters:.2f}%")

