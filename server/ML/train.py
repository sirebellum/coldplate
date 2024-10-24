from construct_model import CNNAutoencoder
from ir_simulator import MLX90640Simulator
import torch
from PIL import Image

def training_loop(model, simulator, batch_size, epochs):
    """
    Train the model on the simulated dataset.

    Args:
        model (CNNAutoencoder): The model to train.
        simulator (MLX90640Simulator): The simulator to generate the training data.
        batch_size (int): Number of samples per batch.
        epochs (int): Number of epochs to train.
    """
    # Get the optimizer
    optimizer = model.optimizer
    model = model.to("cuda")

    # Training loop
    for epoch in range(epochs):
        for i in range(128):
            # Get a batch of heatmaps
            batch = simulator.generate_batch(batch_size)

            # Convert to PyTorch tensor
            batch = torch.tensor(batch, dtype=torch.float32).to("cuda")

            # Size correctly
            batch = batch.unsqueeze(1)

            # Zero the gradients
            optimizer.zero_grad()

            # Forward pass
            output = model(batch)

            # Compute the loss
            loss = model.loss_function(output, batch)

            # Backward pass
            loss.backward()

            # Update the weights
            optimizer.step()

            # Print the loss
            print(f"Epoch {epoch + 1}, Batch {i + 1}: Loss = {loss.item()}")

        # Save example output to image file
        example_output = output[0].squeeze().cpu().detach().numpy()
        example_output = (example_output * 255).astype("uint8")
        example_output = Image.fromarray(example_output)
        example_output = example_output.resize((255, 255))
        example_output.save(f"example_output_{epoch + 1}.jpg")

    # Print completion message
    print("Training complete!")

# Main execution
if __name__ == "__main__":
    
    # Load the model
    model = CNNAutoencoder("model_yamls/visualizer.yaml")

    # Print model summary
    print(model)

    # Get the output size of the encoder
    print(model.encoder_output_size)

    # Get dataset
    simulator = MLX90640Simulator()

    # Train the model
    training_loop(model, simulator, batch_size=32, epochs=10)

    # Save the trained model
    model = model.to("cpu")
    model.save_weights("trained_weights.pth")