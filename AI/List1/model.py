import tensorflow as tf
from dataset_loader import load_emnist

# LOAD emnist/mnist
(x_train, y_train), (x_test, y_test) = load_emnist()

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