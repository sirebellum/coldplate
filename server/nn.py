import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, TensorDataset
from ir_simulator import MLX90640Simulator
from sklearn.metrics import adjusted_rand_score, normalized_mutual_info_score, fowlkes_mallows_score

# Neural network parameters
input_dim = 192  # Assuming 32x24 images
hidden_dim = 32  # Number of neurons in the hidden layer
output_dim = 5  # Number of classes

# Neural network model definition
class SimpleNN(nn.Module):
    def __init__(self, input_dim, hidden_dim, output_dim):
        super(SimpleNN, self).__init__()
        self.fc1 = nn.Linear(input_dim, hidden_dim, bias=False)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(hidden_dim, output_dim, bias=False)
    
    def forward(self, x):
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        return x

def calculate_max_scaling_factor(data):
    # Find the maximum value in the centroids
    max_value = torch.max(data)
    
    # Start with a high scaling factor and adjust down if necessary
    scaling_factor = 1
    max_int_value = 2**31 - 1
    
    # Increase scaling factor to maximize precision until just before overflow
    while True:
        next_scaling_factor = scaling_factor * 10
        max_scaled_value = max_value * next_scaling_factor
        
        if max_scaled_value > max_int_value:
            break
        
        scaling_factor = next_scaling_factor
    
    # Calculate the effective precision achieved
    effective_precision = 1 / scaling_factor
    
    return scaling_factor, effective_precision

def write_quantized_weights_to_file(model, filename):
    with open(filename, "w") as f:
        for param in model.parameters():
            data = param.data.numpy()
            data = data.flatten()
            scaling_factor = calculate_max_scaling_factor(torch.tensor(data))[0]
            data = (data * scaling_factor).astype(int)
            f.write("const int32_t weights[] PROGMEM = {")
            f.write(", ".join(str(x) for x in data))
            f.write("};")
            print("Scaling factor:", scaling_factor)
            print("Effective precision:", 1 / scaling_factor)
            print("Data shape:", data.shape)

def main():
    # Load the dataset
    simulator = MLX90640Simulator()
    images, labels = simulator.generate_batch(10240)
    images_test, labels_test = simulator.generate_batch(1024)

    # Perform 2x2 2x2 max pooling
    images = images.reshape(images.shape[0], 32, 24)
    images_test = images_test.reshape(images_test.shape[0], 32, 24)
    images = images[:, ::2, ::2]
    images_test = images_test[:, ::2, ::2]
    images = images.reshape(images.shape[0], -1)
    images_test = images_test.reshape(images_test.shape[0], -1)

    # Convert to PyTorch tensors
    images_tensor = torch.tensor(images, dtype=torch.float32)
    labels_tensor = torch.tensor(labels, dtype=torch.long)
    images_test_tensor = torch.tensor(images_test, dtype=torch.float32)
    labels_test_tensor = torch.tensor(labels_test, dtype=torch.long)

    # Create a DataLoader for batch processing
    dataset = TensorDataset(images_tensor, labels_tensor)
    dataloader = DataLoader(dataset, batch_size=64, shuffle=True)

    # Initialize the neural network, loss function, and optimizer
    model = SimpleNN(input_dim=input_dim, hidden_dim=hidden_dim, output_dim=output_dim)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001)

    # Training loop
    num_epochs = 10
    for epoch in range(num_epochs):
        running_loss = 0.0
        for inputs, targets in dataloader:
            optimizer.zero_grad()  # Reset gradients
            inputs, targets = inputs, targets
            outputs = model(inputs)  # Forward pass
            loss = criterion(outputs, targets)  # Compute loss
            loss.backward()  # Backward pass
            optimizer.step()  # Update weights
            running_loss += loss.item()
        
        print(f"Epoch {epoch + 1}/{num_epochs}, Loss: {running_loss / len(dataloader)}")

    # Evaluate on labeled images
    with torch.no_grad():
        outputs = model(images_test_tensor)
        _, predictions = torch.max(outputs, 1)
        predictions = predictions.numpy()
        labels_cpu = labels_test_tensor.numpy()

    # Print each weight in the quantized model
    # Print as a c++ array in hex
    write_quantized_weights_to_file(model, "weights.h")

    # Print detailed accuracy information
    print("Adjusted Rand Index:", adjusted_rand_score(labels_cpu, predictions))
    print("Normalized Mutual Information:", normalized_mutual_info_score(labels_cpu, predictions))
    print("Fowlkes-Mallows Index:", fowlkes_mallows_score(labels_cpu, predictions))

if __name__ == "__main__":
    main()
