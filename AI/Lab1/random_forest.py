from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix # download scikit-learn
from dataset_loader import load_emnist

# LOAD emnist/mnist
(x_train_img, y_train), (x_test_img, y_test) = load_emnist()

# flatten arrays (28, 28) into lines. shape[0] - how many images. -1 - flatten 28x28 grid
x_train = x_train_img.reshape(x_train_img.shape[0], -1)
x_test = x_test_img.reshape(x_test_img.shape[0], -1)

# n_estimators - no. trees (default 100) | n_jobs - no. jobs to run in parallel | There are 784 features
forest = RandomForestClassifier(n_jobs = 20)
forest.fit(x_train, y_train)


# GET statistics
y_pred_labels = forest.predict(x_test) # get labels of predictions
confusion = confusion_matrix(y_test, y_pred_labels)

accuracy  = accuracy_score(y_test, y_pred_labels) * 100 # Accuracy is (Correct predictions) / (All predictions)

# Precision: (TP) / (TP + FP). precision measures the model's ability to identify instances of a particular class correctly.
precision = confusion.diagonal() / confusion.sum(axis=0) * 100
# Sens (recall): (TP) / (TP + FN) -  fraction of instances in a class that the model correctly classified out of all instances in that class.
sensitivity = confusion.diagonal() / confusion.sum(axis=1) * 100

print(f"Accuracy: {accuracy:.2f}%")
print("\n| Class | Precision | Sensitivity |")
for i, (p, s) in enumerate(zip(precision, sensitivity)):
    print(f'| {i} | {p:.2f}% | {s:.2f}%')