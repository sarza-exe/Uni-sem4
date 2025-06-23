import tensorflow_datasets as tfds
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt #visualization of digits

    #LOAD DATASET
# Load the EMNIST dataset
#Note: Like the original EMNIST data, images provided here are inverted horizontally and rotated 90 anti-clockwise. You can use tf.transpose within ds.map to convert the images to a human-friendlier format.
emnist, info = tfds.load('emnist/mnist', with_info=True, as_supervised=True)

# Split the dataset into training and testing
train_data, test_data = emnist['train'], emnist['test']

# Convert the iterators to lists
train_list = list(tfds.as_numpy(train_data))
test_list = list(tfds.as_numpy(test_data))

# Unzip the lists into separate tuples for images and labels
x_train, y_train = zip(*train_list)
x_test, y_test = zip(*test_list)

# Convert tuples to NumPy arrays
x_train, y_train = np.array(x_train), np.array(y_train)
x_test, y_test = np.array(x_test), np.array(y_test)

x_train = tf.keras.utils.normalize(x_train, axis=1)
x_test = tf.keras.utils.normalize(x_test, axis=1)

for index in range(10):
    plt.imshow(x_train[index+2].squeeze(), cmap='gray')
    plt.title(f"Label: {y_train[index+2]}")
    plt.axis('off')
    plt.show()

# Print shapes to verify
print(f"x_train shape: {x_train.shape}, y_train shape: {y_train.shape}")
print(f"x_test shape: {x_test.shape}, y_test shape: {y_test.shape}")

