import numpy as np
import tensorflow as tf
import tensorflow_datasets as tfds

def load_emnist():
    """Loads the EMNIST dataset and returns it as normalized NumPy arrays."""

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

    # scales pixel values to values (0,1)
    x_train = tf.keras.utils.normalize(x_train, axis=1)
    x_test = tf.keras.utils.normalize(x_test, axis=1)

    return (x_train, y_train), (x_test, y_test)
