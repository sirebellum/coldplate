import torch
import torch.nn as nn
import torch.optim as optim
import torch.utils.data as data
import numpy as np
import os
from torchvision import transforms
from torch.utils.data import DataLoader, Dataset

# Parameters
input_shape = (1, 32, 24)  # Single channel, 32x24 resolution
noise_stddev = 0.1  # Standard deviation for Gaussian noise
batch_size = 64
epochs = 50
learning_rate = 1e-3
model_path = 'autoencoder_weights.pth'

# Custom dataset for IR frames with noise addition
class IRDataset(Dataset):
    def __init__(self, frames, transform=None, add_noise=True):
        self.frames = frames
        self.transform = transform
        self.add_noise = add_noise

    def __len__(self):
        return len(self.frames)

    def __getitem__(self, idx):
        frame = self.frames[idx]

        if self.transform:
            frame = self.transform(frame)

        if self.add_noise:
            noise = torch.randn(frame.size()) * noise_stddev
            frame = frame + noise
            frame = torch.clamp(frame, 0., 1.)  # Keep values in range [0, 1]

        return frame, frame  # Autoencoder target is the original frame

# Define the CNN autoencoder
class CNNAutoencoder(nn.Module):
    def __init__(self):
        super(CNNAutoencoder, self).__init__()
        # Encoder
        self.encoder = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, stride=2, padding=1),  # Output: 16 x 16 x 12
            nn.ReLU(),
            nn.Conv2d(16, 32, kernel_size=3, stride=2, padding=1),  # Output: 32 x 8 x 6
            nn.ReLU(),
            nn.Conv2d(32, 64, kernel_size=3, stride=2, padding=1),  # Output: 64 x 4 x 3
            nn.ReLU()
        )
        # Decoder
        self.decoder = nn.Sequential(
            nn.ConvTranspose2d(64, 32, kernel_size=3, stride=2, padding=1, output_padding=(1, 1)),  # 32 x 8 x 6
            nn.ReLU(),
            nn.ConvTranspose2d(32, 16, kernel_size=3, stride=2, padding=1, output_padding=(1, 1)),  # 16 x 16 x 12
            nn.ReLU(),
            nn.ConvTranspose2d(16, 1, kernel_size=3, stride=2, padding=1, output_padding=(1, 0)),  # 1 x 32 x 24
            nn.Sigmoid()  # Output scaled to [0, 1]
        )

    def forward(self, x):
        encoded = self.encoder(x)
        decoded = self.decoder(encoded)
        return decoded

# Training loop
def train_autoencoder(model, dataloader, criterion, optimizer, epochs, device):
    model.train()
    for epoch in range(epochs):
        running_loss = 0.0
        for data in dataloader:
            inputs, targets = data
            inputs, targets = inputs.to(device), targets.to(device)

            optimizer.zero_grad()
            outputs = model(inputs)
            loss = criterion(outputs, targets)
            loss.backward()
            optimizer.step()

            running_loss += loss.item()

        print(f'Epoch [{epoch+1}/{epochs}], Loss: {running_loss/len(dataloader):.4f}')

    print('Training complete.')

# Save model weights
def save_model(model, path):
    torch.save(model.state_dict(), path)
    print(f'Model weights saved to {path}')

# Main execution
if __name__ == "__main__":
    # Dummy IR frames for testing (replace with actual data)
    num_samples = 1000
    frames = np.random.rand(num_samples, *input_shape).astype(np.float32)

    # Transforms
    transform = transforms.Compose([
        transforms.ToTensor(),
    ])

    # Create dataset and dataloader
    dataset = IRDataset(frames, transform=transform, add_noise=True)
    dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True)

    # Initialize model, loss function, and optimizer
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = CNNAutoencoder().to(device)
    criterion = nn.MSELoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)

    # Train the model
    train_autoencoder(model, dataloader, criterion, optimizer, epochs, device)

    # Save the trained model weights
    save_model(model, model_path)
