import os
import cv2 #computer vision, load and process images
import numpy as np
import matplotlib.pyplot as plt #visualization of digits
import tensorflow as tf
from dataset_loader import load_emnist

# LOAD emnist/mnist
(x_train, y_train), (x_test, y_test) = load_emnist()

    # CHECK MODEL ON HAND-DRAWN DIGITS
model = tf.keras.models.load_model('numbers.keras')

correct = 0
total = 0
arr = []
for filename in os.listdir('digits'):
        if filename.endswith(".png"):
            label = int(filename.split('.')[0][1:]) # filename = g5.123.png -> label = 5

            img = cv2.imread(os.path.join('digits', filename))[:,:,0]

            img = cv2.flip(img, 1) # Invert horizontally
            img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE) # Rotate 90 degrees counterclockwise

            img = np.invert(np.array([img])) #invert black and white
            img = tf.keras.utils.normalize(img, axis=1)

            prediction = model.predict(img)
            prediction_label = np.argmax(prediction)
            arr.append(prediction_label)
            total += 1
            if prediction_label == label:
                correct += 1

            print("This ", label, " is probably a", prediction_label)

            # plt.imshow(img[0], cmap='gray')
            # plt.show()

index = 0
for i in range(10):
    print(f"\n| {i} ", end='')
    for x in range(1,4):
        print(f"| {arr[index]} ", end='')
        index += 1

print("Accuracy is", correct/total)


