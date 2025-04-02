import os
import cv2 #computer vision, load and process images
import numpy as np #arrays
import matplotlib.pyplot as plt #visualization of digits
import tensorflow as tf #training
import tensorflow_datasets as tfds

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


    # CHECK MODEL ON HAND-DRAWN DIGITS
model = tf.keras.models.load_model('numbers.keras')

correct = 0
for filename in os.listdir('digits'):
        if filename.endswith(".png"):
            label = int(filename.split('.')[0][1:]) # filename = g5.123.png -> label = 5

            img = cv2.imread(os.path.join('digits', filename))[:,:,0]

            img = cv2.flip(img, 1) # Invert horizontally
            img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE) # Rotate 90 degrees counterclockwise

            img = np.invert(np.array([img])) #invert black and white

            prediction = model.predict(img)
            prediction_label = np.argmax(prediction)
            if prediction_label == label:
                correct += 1

            # plt.imshow(img[0], cmap='gray')
            # plt.show()

print("Accuracy is", correct/30)


