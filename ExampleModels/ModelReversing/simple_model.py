import tensorflow as tf
from tensorflow import keras
import numpy as np

def xy_train():
    x_train = []
    y_train = []    
    for i in range(10000):
        x_train.append([i] * 16)  # replicate i to fit shape (784,)
        y_train.append(i % 10)     # example classification target (0–9)
    return np.array(x_train, dtype=np.float32), keras.utils.to_categorical(y_train, num_classes=10)

def xy_validate():
    x_val = []
    y_val = []    
    for i in range(200):
        x_val.append([i] * 16)
        y_val.append(i % 10)
    return np.array(x_val, dtype=np.float32), keras.utils.to_categorical(y_val, num_classes=10)

# Define the model
model = keras.Sequential([
    keras.layers.Dense(32, activation='relu', input_shape=(16,)),
    keras.layers.Dense(10, activation='softmax')
])

# Compile the model
model.compile(optimizer='adam',
              loss='categorical_crossentropy',
              metrics=['accuracy'])

x_train, y_train = xy_train()
x_val, y_val = xy_validate()

# Train the model
model.fit(x_train, y_train, epochs=10, validation_data=(x_val, y_val))

# Save model
model.save('model.h5')

model.summary()