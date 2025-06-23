import os
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

def _sigmoid(x):
    return 1 / (1 + np.exp(-x))

def _sigmoid_derivative(x):
    return x * (1 - x)

def _relu(x):
    return np.maximum(0, x)

def _relu_derivative(x):
    return (x > 0).astype(float)


class NeuralNetwork:
    def __init__(self, input_size, hidden_size, output_size, activation_function='sigmoid'):
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.output_size = output_size

        self.weights_in_hid = np.random.randn(self.hidden_size, self.input_size) * 0.01
        self.bias_hid = np.zeros((self.hidden_size, 1))

        self.weights_hid_out = np.random.randn(self.output_size, self.hidden_size) * 0.01
        self.bias_out = np.zeros((self.output_size, 1))

        self.activation_function = activation_function

    def forward(self, X):
        self.hidden_input = np.dot(self.weights_in_hid, X) + self.bias_hid
        if self.activation_function == 'relu':
            self.hidden_output = _relu(self.hidden_input)
        else: # 'sigmoid'
            self.hidden_output = _sigmoid(self.hidden_input)

        self.output_input = np.dot(self.weights_hid_out, self.hidden_output) + self.bias_out
        if self.activation_function == 'relu':
            self.predicted_output = _relu(self.output_input)
        else: # 'sigmoid'
            self.predicted_output = _sigmoid(self.output_input)

        return self.predicted_output

    def backward(self, X, y, learning_rate):
        error = self.predicted_output - y

        if self.activation_function == 'relu':
            d_predicted_output = error * _relu_derivative(self.predicted_output)
        else: # 'sigmoid'
            d_predicted_output = error * _sigmoid_derivative(self.predicted_output)

        d_weights_hid_out = np.dot(d_predicted_output, self.hidden_output.T)
        d_bias_out = d_predicted_output

        error_hidden = np.dot(self.weights_hid_out.T, d_predicted_output)

        if self.activation_function == 'relu':
            d_hidden_output = error_hidden * _relu_derivative(self.hidden_output)
        else: # 'sigmoid'
            d_hidden_output = error_hidden * _sigmoid_derivative(self.hidden_output)

        d_weights_in_hid = np.dot(d_hidden_output, X.T)
        d_bias_hid = d_hidden_output

        self.weights_hid_out -= learning_rate * d_weights_hid_out
        self.bias_out -= learning_rate * d_bias_out
        self.weights_in_hid -= learning_rate * d_weights_in_hid
        self.bias_hid -= learning_rate * d_bias_hid

    def train(self, X_train, y_train, epochs, learning_rate):
        losses = [] # List to store loss values per epoch
        for epoch in range(epochs):
            total_error = 0
            for i in range(X_train.shape[1]):
                X = X_train[:, i:i+1]
                y = y_train[:, i:i+1]

                predicted_output = self.forward(X)
                self.backward(X, y, learning_rate)
                total_error += np.mean(np.square(y - predicted_output)) # Mean Squared Error (MSE)
            losses.append(total_error / X_train.shape[1]) # Average MSE for the epoch
        return losses

    def predict(self, X):
        predictions = self.forward(X)
        return (predictions >= 0.5).astype(int)

def generate_data(num_samples=1000):
    X = np.random.uniform(-1, 1, (2, num_samples))
    X[X == 0] = np.random.uniform(-1, 1, np.sum(X == 0))
    y = np.zeros((1, num_samples))
    for i in range(num_samples):
        if (X[0, i] > 0 and X[1, i] > 0) or (X[0, i] < 0 and X[1, i] < 0):
            y[0, i] = 1
    return X, y

def normalize_l1(X):
    norm_l1 = np.sum(np.abs(X), axis=0, keepdims=True)
    norm_l1[norm_l1 == 0] = 1
    return X / norm_l1

def normalize_l2(X):
    norm_l2 = np.linalg.norm(X, axis=0, keepdims=True)
    norm_l2[norm_l2 == 0] = 1
    return X / norm_l2


# Training parameters
epochs = 100 #500
learning_rates = [0.001, 0.01, 0.1, 1.0]

X_raw_train, y_train = generate_data(num_samples=1000)
X_raw_test, y_test = generate_data(num_samples=200)

X_l1_train = normalize_l1(X_raw_train)
X_l2_train = normalize_l2(X_raw_train)

X_l1_test = normalize_l1(X_raw_test)
X_l2_test = normalize_l2(X_raw_test)

all_results = []
loss_histories = {}

def run_experiment_and_collect_losses(activation_func, X_train_data, y_train_data, X_test_data, y_test_data, lr, norm_name):
    nn = NeuralNetwork(input_size=2, hidden_size=4, output_size=1, activation_function=activation_func)
    epoch_losses = nn.train(X_train_data, y_train_data, epochs=epochs, learning_rate=lr)

    train_predictions = nn.predict(X_train_data)
    train_accuracy = np.mean(train_predictions == y_train_data.astype(int)) * 100

    test_predictions = nn.predict(X_test_data)
    test_accuracy = np.mean(test_predictions == y_test_data.astype(int)) * 100

    loss_histories[(activation_func.capitalize(), norm_name, lr)] = epoch_losses

    return train_accuracy, test_accuracy

print("--- Starting experiments ---")

normalization_types = {
    'Unnormalized': (X_raw_train, X_raw_test),
    'L1 Normalized': (X_l1_train, X_l1_test),
    'L2 Normalized': (X_l2_train, X_l2_test)
}
activation_functions = ['sigmoid', 'relu']

for activation_func in activation_functions:
    print(f"\n--- Activation Function: {activation_func.capitalize()} ---")
    for norm_name, (X_train_norm, X_test_norm) in normalization_types.items():
        print(f"  Configuration: {norm_name}")
        for lr in learning_rates:
            print(f"    Testing for learning rate: {lr}")
            train_acc, test_acc = run_experiment_and_collect_losses(activation_func, X_train_norm, y_train, X_test_norm, y_test, lr, norm_name)
            print(f"      Train Accuracy: {train_acc:.2f}%, Test Accuracy: {test_acc:.2f}%")

            all_results.append({
                'Activation': activation_func.capitalize(),
                'Normalization': norm_name,
                'Learning Rate': lr,
                'Data Split': 'Train',
                'Accuracy': train_acc
            })
            all_results.append({
                'Activation': activation_func.capitalize(),
                'Normalization': norm_name,
                'Learning Rate': lr,
                'Data Split': 'Test',
                'Accuracy': test_acc
            })

print("\n--- All experiments finished. Generating plots ---")

df = pd.DataFrame(all_results)

train_df = df[df['Data Split'] == 'Train'].copy()
train_df['Config'] = train_df['Activation'] + '\n' + '(' + train_df['Normalization'] + ')'
train_pivot = train_df.pivot_table(index='Config', columns='Learning Rate', values='Accuracy')

test_df = df[df['Data Split'] == 'Test'].copy()
test_df['Config'] = test_df['Activation'] + '\n' + '(' + test_df['Normalization'] + ')'
test_pivot = test_df.pivot_table(index='Config', columns='Learning Rate', values='Accuracy')


# Plotting Heatmaps
def plot_single_heatmap(pivot_table, title, filename):
    plt.figure(figsize=(10, 8))
    sns.heatmap(pivot_table, annot=True, fmt=".1f", cmap="YlGnBu", linewidths=.5, cbar_kws={'label': 'Accuracy (%)'})
    plt.title(title, fontsize=16, fontweight='bold')
    plt.xlabel('Learning Rate', fontsize=12)
    plt.ylabel('Configuration', fontsize=12)
    plt.yticks(rotation=0)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()


def plot_loss_curves(loss_histories, epochs, filename):
    fig, axes = plt.subplots(len(activation_functions), len(normalization_types), figsize=(18, 12), sharex=True, sharey=False)
    axes = axes.flatten()

    plot_idx = 0
    for act_func in activation_functions:
        for norm_name in normalization_types.keys():
            ax = axes[plot_idx]
            for lr in learning_rates:
                key = (act_func.capitalize(), norm_name, lr)
                if key in loss_histories:
                    losses = loss_histories[key]
                    ax.plot(range(1, epochs + 1), losses, label=f'{lr}')

            ax.set_title(f'{act_func.capitalize()} ({norm_name})', fontsize=12, fontweight='bold')
            ax.set_xlabel('Epoch', fontsize=10)
            ax.set_ylabel('Loss', fontsize=10)
            ax.grid(True, linestyle='--', alpha=0.6)
            ax.legend(title="Learning Rate", fontsize=8)
            ax.set_ylim(bottom=0)

            plot_idx += 1

    fig.suptitle('Loss Curves Over Epochs for Different Configurations', fontsize=18, fontweight='bold', y=1.02)
    plt.tight_layout(rect=[0, 0, 1, 0.98])
    plt.savefig(filename)


if not os.path.exists("plots"):
        os.makedirs("plots")

plot_single_heatmap(train_pivot, 'Training Accuracy Heatmap', "plots/train_accuracy_heatmap.png")
plot_single_heatmap(test_pivot, 'Test Accuracy Heatmap', "plots/test_accuracy_heatmap.png")
plot_loss_curves(loss_histories, epochs, "plots/loss_comparison.png")

print("\nAll plots successfully generated and saved")