import numpy as np
import cv2
import torch
from models import Session, ImageRecord
from ML.construct_model import CNNAutoencoder

IR_X = 32
IR_Y = 24
IMAGE_DIR = 'images'

# Create a new session
session = Session()

def load_images():
    images = session.query(ImageRecord).all()
    image_data = []
    for img in images:
        file_path = img.file_path
        image = cv2.imread(file_path, cv2.IMREAD_GRAYSCALE)
        if image is None:
            continue
        normalized_image = image / 255.0
        image_data.append(normalized_image.flatten())
    return np.array(image_data)

def training_loop(model, image_data, epochs=10, batch_size=128):
    print("Starting training loop...")
    for epoch in range(epochs):
        batch = image_data[epoch % len(image_data):epoch % len(image_data) + batch_size]
        batch = torch.tensor(batch, dtype=torch.float32)
        batch = batch.reshape(batch.shape[0], 1, IR_Y, IR_X)
        if torch.cuda.is_available():
            batch = batch.to("cuda")
        model.optimizer.zero_grad()
        output = model.forward(batch)
        loss = model.loss_function(output, batch)
        loss.backward()
        model.optimizer.step()
        print(f"Epoch {epoch + 1}: Loss = {loss.item()}")
    print("Training complete!")
    return model

def main():
    image_data = load_images()
    autoencoder = CNNAutoencoder("ML/model_yamls/visualizer.yaml")
    print(autoencoder)
    if torch.cuda.is_available():
        autoencoder = autoencoder.to("cuda")
    autoencoder = training_loop(autoencoder, image_data, epochs=4200, batch_size=128)

    # Save the encoder weights to disk
    torch.save(autoencoder.get_encoder().state_dict(), "encoder_weights.pth")
    print("Encoder weights saved.")

if __name__ == "__main__":
    main()
