from tensorflow import keras

model = keras.models.load_model('model.h5')
import numpy as np
from tensorflow.keras.utils import to_categorical

# Dummy test data
x_test = np.array([[i] * 16 for i in range(100)], dtype=np.float32)
y_test = np.array([i % 10 for i in range(100)])  # class labels: 0–9
y_test = to_categorical(y_test, num_classes=10)

loss, accuracy = model.evaluate(x_test, y_test)
print(f"Test loss: {loss:.4f}, Test accuracy: {accuracy:.4f}")

# Make predictions
predictions = model.predict(x_test)
print(predictions)