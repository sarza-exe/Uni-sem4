from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt
import seaborn as sns
import tensorflow as tf
from sklearn.neighbors import NearestNeighbors
import numpy as np

from metrics import compute_and_print_metrics, compute_and_display_purity

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

    return x_train, y_train

def reduce_dimensions(x_train):
    # We need to reduce the number of dimensions in order for DBSCAN to work correctly
    # DBSCAN has a curse of dimensionality (and 784 is a lot of dimensions)
    print("Applying PCA...")
    pca = PCA(n_components=50, random_state=0) # reduce dimensions from 784 to 50
    pca_res_50 = pca.fit_transform(x_train)

    print("Applying T-SNE...")
    tsne = TSNE(n_components=2, random_state=0, n_jobs=-1, perplexity=30, learning_rate='auto', init='random') # reduce dimensions from 50 to 2
    tsne_res = tsne.fit_transform(pca_res_50)

    print("Dimensions reduced")
    return tsne_res

# Plot k-distance graph to guide eps choice:
def plot_k_distance(X, k):
    from sklearn.neighbors import NearestNeighbors
    nbrs = NearestNeighbors(n_neighbors=k).fit(X)
    distances, _ = nbrs.kneighbors(X)
    # distance to k-th neighbor for each point:
    k_distances = np.sort(distances[:, k-1])
    plt.figure(figsize=(6,4))
    plt.plot(k_distances)
    plt.xlabel(f"Points sorted by distance to {k}-th neighbor")
    plt.ylabel(f"Distance to {k}-th neighbor")
    plt.title(f"k-distance Graph (k={k})")
    plt.grid(True)
    plt.savefig("plots/k_distance.png")
    plt.show()

if __name__ == '__main__':

    # data loading and diemnsions reducing
    x_train, y_train = load_mnist()
    tsne_train = reduce_dimensions(x_train)

    # plot data with real labels
    print("Data plot with real labels:")
    plt.figure(figsize=(10,7))
    sns.scatterplot(x = tsne_train[:,0], y = tsne_train[:,1], hue = y_train, legend = 'full', palette = sns.hls_palette(10))
    plt.title("True labels for datapoints", pad=15)
    plt.savefig("plots/true_labels.png")
    plt.tight_layout()

    print("k-distance graph:")
    min_samples = 11 #7 9
    plot_k_distance(tsne_train, k=min_samples)

    # dbsacan
    eps = 2.0 # 2.4 2.2
    dbscan = Dbscan(eps, min_samples)
    labels = dbscan.fit(tsne_train)

    # plot clusters
    plt.figure(figsize=(10,7))
    unique_labels = np.unique(labels)
    cluster_labels = [lbl for lbl in unique_labels if lbl != -1]
    palette = sns.hls_palette(len(cluster_labels))
    color_map = {lbl: palette[i] for i, lbl in enumerate(cluster_labels)}

    for lbl in unique_labels:
        if lbl == -1:
            color = 'black'
            marker = 'o'
            size = 0.2
            label_name = "Noise"
        else:
            color = color_map[lbl]
            marker = 'o'
            size = 3
            label_name = f"Cluster {lbl}"
        pts = tsne_train[labels == lbl]
        plt.scatter(pts[:,0], pts[:,1], c=[color], s=size, marker=marker, label=label_name)
    plt.legend(markerscale=2, bbox_to_anchor=(1.05,1), loc='upper left', fontsize='small')
    plt.title(f"DBSCAN Clustering (eps={eps}, min_samples={min_samples})")
    plt.tight_layout()
    plt.savefig("plots/clusters.png")
    plt.show()

    compute_and_print_metrics(y_train, labels)
    compute_and_display_purity(labels, y_train)