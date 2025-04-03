import numpy as np
import tensorflow as tf
from dataset_loader import load_emnist
from sklearn.metrics import confusion_matrix # download scikit-learn

#LOAD emnist/mnist
(x_train, y_train), (x_test, y_test) = load_emnist()

#METRICS
model = tf.keras.models.load_model('numbers.keras')

y_pred = model.predict(x_test)
y_pred_labels = np.argmax(y_pred, axis=1) # get labels of predictions

confusion = confusion_matrix(y_test, y_pred_labels)

# Accuracy is (Correct predictions) / (All predictions)
accuracy  = model.evaluate(x_test, y_test)[1] * 100 #[1] bo [0] to loss

# Precision is (TP) / (TP + FP). precision measures the model's ability to identify instances of a particular class correctly.
precision = confusion.diagonal() / confusion.sum(axis=0) * 100
# Sens (recall): (TP) / (TP + FN) -  fraction of instances in a class that the model correctly classified out of all instances in that class.
sensitivity = confusion.diagonal() / confusion.sum(axis=1) * 100

print(f"Accuracy: {accuracy:.2f}%")

print("\n| Class | Precision | Sensitivity |")
for i, (p, s) in enumerate(zip(precision, sensitivity)):
    print(f'| {i} | {p:.2f}% | {s:.2f}%')

