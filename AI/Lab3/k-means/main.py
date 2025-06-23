import numpy as np
from tensorflow import keras
import matplotlib.pyplot as plt
import ctypes

class MiniBatchKMeans:
    def __init__(self, n_clusters=10, batch_size=100, max_iter=100, init_size=None, verbose=False):
        self.n_clusters = n_clusters
        self.batch_size = batch_size
        self.max_iter = max_iter
        self.init_size = init_size or 3 * n_clusters * batch_size
        self.verbose = verbose
        self.cluster_centers_ = None
        self.counts_ = None

    def _kmeans_plusplus_init(self, X):
        """Initialize centroids using the k-means++ algorithm."""
        n_samples, _ = X.shape
        centers = np.empty((self.n_clusters, X.shape[1]), dtype=X.dtype)
        # first centroid
        first_idx = np.random.randint(0, n_samples)
        centers[0] = X[first_idx]
        # compute squared distances of x to the first center
        closest_dist_sq = np.sum((X - centers[0])**2, axis=1)
        for c in range(1, self.n_clusters):
            probs = closest_dist_sq / closest_dist_sq.sum()
            next_idx = np.random.choice(n_samples, p=probs)
            centers[c] = X[next_idx]
            # update distances
            dist_sq = np.sum((X - centers[c])**2, axis=1)
            closest_dist_sq = np.minimum(closest_dist_sq, dist_sq)
        return centers

    def partial_fit(self, X_batch):
        """Update centroids based on a batch of samples."""
        if self.cluster_centers_ is None:
            init_subset = X_batch if X_batch.shape[0] >= self.init_size else X_batch
            self.cluster_centers_ = self._kmeans_plusplus_init(init_subset)
            self.counts_ = np.zeros(self.n_clusters, dtype=int)

        # assign batch to nearest centroids
        distances = np.linalg.norm(X_batch[:, np.newaxis] - self.cluster_centers_, axis=2)
        labels = np.argmin(distances, axis=1)
        # update centroids with incremental average
        for idx, x in zip(labels, X_batch):
            self.counts_[idx] += 1
            eta = 1.0 / self.counts_[idx]
            self.cluster_centers_[idx] = (1 - eta) * self.cluster_centers_[idx] + eta * x

    def fit(self, X):
        """Run mini-batch k-means over the data."""
        n_samples = X.shape[0]
        for it in range(self.max_iter):
            perm = np.random.permutation(n_samples)
            for i in range(0, n_samples, self.batch_size):
                batch_idx = perm[i:i + self.batch_size]
                self.partial_fit(X[batch_idx])
            if self.verbose:
                print(f"Iteration {it+1}/{self.max_iter} completed.")
        return self

    def predict(self, X):
        """Assign clusters for input samples."""
        distances = np.linalg.norm(X[:, np.newaxis] - self.cluster_centers_, axis=2)
        return np.argmin(distances, axis=1)

    def inertia(self, X):
        """Compute the sum of squared distances to the nearest centroid."""
        distances_sq = np.min(
            np.sum((X[:, np.newaxis, :] - self.cluster_centers_)**2, axis=2),
            axis=1
        )
        return np.sum(distances_sq)


def load_mnist():
    (x_train, y_train), (x_test, y_test) = keras.datasets.mnist.load_data()
    X = np.vstack((x_train, x_test)).reshape(-1, 28*28).astype(np.float32) / 255.0
    y = np.hstack((y_train, y_test))
    return X, y


# Plotting Functions
def plot_allocation_matrix(labels, y, n_clusters):
    counts = np.zeros((n_clusters, 10), int)
    for lbl, digit in zip(labels, y):
        counts[lbl, digit] += 1
    percent = counts / counts.sum(axis=1, keepdims=True) * 100
    fig, ax = plt.subplots(figsize=(8, n_clusters*0.6))
    im = ax.imshow(percent, aspect='equal')
    ax.set_xlabel('True Digit')
    ax.set_ylabel('Cluster Index')
    ax.set_title(f'Digit Allocation (%) for {n_clusters} Clusters')
    ax.set_xticks(range(10)); ax.set_yticks(range(n_clusters))
    ax.set_xticklabels(range(10)); ax.set_yticklabels(range(n_clusters))
    for i in range(n_clusters):
        for j in range(10):
            ax.text(j, i, f"{percent[i,j]:.1f}%", ha='center', va='center', fontsize=6)
    fig.colorbar(im, ax=ax, label='% of cluster')
    plt.tight_layout()
    plt.savefig(f'plots/matrix_{n_clusters}_clusters.png')
    plt.show()


def plot_centroids(centers):
    n_clusters = centers.shape[0]
    cols = 5
    rows = (n_clusters) // cols
    fig, axes = plt.subplots(rows, cols, figsize=(cols, rows))
    axes = axes.flatten()
    for i, center in enumerate(centers):
        axes[i].imshow(center.reshape(28, 28), cmap='gray')
        axes[i].set_title(f'C {i}', fontsize=8)
        axes[i].axis('off')
    for ax in axes[n_clusters:]:
        ax.remove()
    plt.suptitle(f'{n_clusters} Centroids')
    plt.tight_layout()
    plt.savefig(f'plots/centroids_{n_clusters}_clusters.png')
    plt.show()


# Experiment Runner
def run_for_k(X, y, k, n_runs, batch_size, max_iter, verbose):
    best_inertia = np.inf
    best_centers = None
    for run in range(n_runs):
        if verbose: print(f"k={k}, run {run+1}/{n_runs}")
        model = MiniBatchKMeans(n_clusters=k, batch_size=batch_size, max_iter=max_iter, verbose=False)
        model.fit(X)
        iner = model.inertia(X)
        if verbose: print(f" Inertia: {iner:.2f}")
        if iner < best_inertia:
            best_inertia = iner
            best_centers = model.cluster_centers_.copy()
    model.cluster_centers_ = best_centers
    labels = model.predict(X)
    return labels, best_centers, best_inertia


# Main
if __name__ == '__main__':

    X, y = load_mnist()
    print(f"Data loaded: {X.shape[0]} samples, {X.shape[1]} features.")

    runs = 10
    batch = 256
    iters = 100
    ks = [10, 15, 20, 30]

    for k in ks:
        labels, centers, inertia = run_for_k(X, y, k, runs, batch, iters, False)
        print(f"k={k} best inertia: {inertia:.2f}")
        plot_allocation_matrix(labels, y, k)
        plot_centroids(centers)