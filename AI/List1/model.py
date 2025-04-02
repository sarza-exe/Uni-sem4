import numpy as np #arrays
import tensorflow as tf #training
import tensorflow_datasets as tfds

    #LOADING DATASET
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

#scales pixel values to values (0,1)
x_train = tf.keras.utils.normalize(x_train, axis=1)
x_test = tf.keras.utils.normalize(x_test, axis=1)



    #MODEL
model = tf.keras.models.Sequential()

#layers in our model
model.add(tf.keras.layers.Flatten(input_shape=(28, 28)))#flatten turns 28x28 grid into a line of 28^2 elements
model.add(tf.keras.layers.Dense(128, activation='relu')) # max(0,a)
model.add(tf.keras.layers.Dense(128, activation='relu')) # max(0,a)
model.add(tf.keras.layers.Dense(10, activation='softmax')) # softmax return probability (0,1) and all 10 neurons add up to 1

model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['accuracy'])

model.fit(x_train, y_train, epochs=3) # how many times is the model going to see the same numbers all over again

model.save('numbers.keras')