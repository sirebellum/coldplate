import yaml
import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
import torch.nn.functional as F

class KMeansLayer(nn.Module):
    def __init__(self, num_clusters, num_features, max_iters=100, tol=1e-4):
        super(KMeansLayer, self).__init__()
        self.num_clusters = num_clusters
        self.num_features = num_features
        self.max_iters = max_iters
        self.tol = tol
        
        # Initialize cluster centroids as learnable parameters
        self.centroids = nn.Parameter(torch.randn(num_clusters, num_features))

    def forward(self, x):
        """
        Perform KMeans clustering in the forward pass.
        
        Args:
            x (torch.Tensor): Input data, shape (batch_size, num_features)
            
        Returns:
            cluster_assignments (torch.Tensor): Cluster indices, shape (batch_size,)
            distances (torch.Tensor): Distance to closest centroid, shape (batch_size,)
        """
        # Reshape centroids for broadcasting
        centroids_expanded = self.centroids.unsqueeze(0)  # (1, num_clusters, num_features)
        x_expanded = x.unsqueeze(1)  # (batch_size, 1, num_features)
        
        # Calculate the Euclidean distances to each centroid
        distances = torch.norm(x_expanded - centroids_expanded, dim=2)  # (batch_size, num_clusters)
        
        # Assign clusters based on the nearest centroid
        # cluster_assignments = torch.argmin(distances, dim=1)  # (batch_size,)

        return distances

    def fit(self, x):
        """
        Fit the KMeans centroids using the input data.
        
        Args:
            x (torch.Tensor): Input data, shape (num_samples, num_features)
        """
        # Randomly initialize centroids from the input data
        indices = torch.randperm(x.size(0))[:self.num_clusters]
        self.centroids.data = x[indices].clone()

        for _ in range(self.max_iters):
            # Compute distances and assign clusters
            cluster_assignments, distances = self.forward(x)
            new_centroids = torch.zeros_like(self.centroids)

            # Calculate new centroids based on the mean of assigned points
            for k in range(self.num_clusters):
                points_in_cluster = x[cluster_assignments == k]
                if len(points_in_cluster) > 0:
                    new_centroids[k] = points_in_cluster.mean(dim=0)

            # Check for convergence
            centroid_shift = torch.norm(self.centroids - new_centroids, dim=1).mean()
            self.centroids.data = new_centroids
            if centroid_shift < self.tol:
                break

class CNNAutoencoder(nn.Module):
    def __init__(self, config_path):
        super(CNNAutoencoder, self).__init__()
        self.config = self._load_config(config_path)
        self.encoder = self._build_encoder()
        self.decoder = self._build_decoder()
        self.encoder_output_size = self._get_encoder_output_size()
        self.kmeans = self._build_kmeans()

        self.autoencoder = nn.Sequential(self.encoder, self.kmeans, self.decoder)
        self._compile()

    def _get_encoder_output_size(self):
        with torch.no_grad():
            dummy_input = torch.randn(1, *self.config['model']['input_shape'])
            encoder_output = self.encoder(dummy_input)
            return encoder_output.shape[1:]

    def _load_config(self, config_path):
        with open(config_path, 'r') as file:
            return yaml.safe_load(file)

    def _build_kmeans(self):
        layers = self.config['model']['kmeans']['layers']
        kmeans_layers = []
        
        in_channels = self.encoder_output_size[0]
        
        for layer in layers:
            kmeans_layers += self._add_layer(layer, in_channels)
            if layer['type'] == 'Conv2D':
                in_channels = layer['filters']
            if layer['type'] == 'Flatten':
                in_channels = np.prod(in_channels)
            elif layer['type'] == 'Reshape':
                in_channels = layer['target_shape'][0]
            elif layer['type'] == 'Dense':
                in_channels = layer['units_out']
            elif layer['type'] == 'MaxPooling2D':
                in_channels = in_channels
            elif layer['type'] == 'BatchNormalization':
                in_channels = in_channels
            elif layer['type'] == 'KMeans':
                in_channels = in_channels
            elif layer['type'] == 'DepthwiseConv2D':
                in_channels = in_channels * layer['groups']

        return nn.Sequential(*kmeans_layers)

    def _build_encoder(self):
        layers = self.config['model']['encoder']['layers']
        input_shape = tuple(self.config['model']['input_shape'])
        encoder_layers = []
        
        in_channels = input_shape[0]
        
        for layer in layers:
            encoder_layers += self._add_layer(layer, in_channels)
            if layer['type'] == 'Conv2D':
                in_channels = layer['filters']
            if layer['type'] == 'Flatten':
                in_channels = np.prod(in_channels)
            elif layer['type'] == 'Reshape':
                in_channels = layer['target_shape'][0]
            elif layer['type'] == 'Dense':
                in_channels = layer['units_out']
            elif layer['type'] == 'MaxPooling2D':
                in_channels = in_channels
            elif layer['type'] == 'BatchNormalization':
                in_channels = in_channels
            elif layer['type'] == 'KMeans':
                in_channels = in_channels
            elif layer['type'] == 'DepthwiseConv2D':
                in_channels = in_channels * layer['groups']

        return nn.Sequential(*encoder_layers)

    def _build_decoder(self):
        layers = self.config['model']['decoder']['layers']
        
        # Infer in_channels from the encoder's output shape
        with torch.no_grad():
            dummy_input = torch.randn(1, *self.config['model']['input_shape'])
            encoder_output = self.encoder(dummy_input)
            in_channels = encoder_output.shape[1]  # Get channels from encoder's output

        decoder_layers = []
        
        for layer in layers:
            decoder_layers += self._add_layer(layer, in_channels)
            if layer['type'] == 'Conv2DTranspose':
                in_channels = layer['filters']
            if layer['type'] == 'Conv2D':
                in_channels = layer['filters']
            elif layer['type'] == 'UpSampling2D':
                in_channels = in_channels
            elif layer['type'] == 'Flatten':
                in_channels = np.prod(in_channels)
            elif layer['type'] == 'Reshape':
                in_channels = layer['target_shape'][0]
            elif layer['type'] == 'Dense':
                in_channels = layer['units_out']
            elif layer['type'] == 'MaxPooling2D':
                in_channels = in_channels
            elif layer['type'] == 'BatchNormalization':
                in_channels = in_channels
            elif layer['type'] == 'KMeans':
                in_channels = in_channels
            elif layer['type'] == 'DepthwiseConv2D':
                in_channels = in_channels * layer['groups']
                

        return nn.Sequential(*decoder_layers)

    def _add_layer(self, layer_config, in_channels):
        layer_type = layer_config['type']
        layers = []

        if layer_type == 'Conv2D':
            padding = layer_config['padding']
            
            # Handle padding strings
            if isinstance(padding, str):
                if padding == 'same':
                    padding = 'same'
                elif padding == 'valid':
                    padding = 0
                else:
                    raise ValueError(f"Unsupported padding type: {padding}")
            else:
                padding = tuple(padding)
            
            layers.append(nn.Conv2d(
                in_channels=in_channels,
                out_channels=layer_config['filters'],
                kernel_size=tuple(layer_config['kernel_size']),
                stride=tuple(layer_config['stride']),
                padding=self._calculate_padding(padding, layer_config['kernel_size'], layer_config['stride'])
            ))
            layers.append(self._get_activation(layer_config['activation']))

        elif layer_type == 'DepthwiseConv2D':
            padding = layer_config['padding']
            
            # Handle padding strings
            if isinstance(padding, str):
                if padding == 'same':
                    padding = 'same'
                elif padding == 'valid':
                    padding = 0
                else:
                    raise ValueError(f"Unsupported padding type: {padding}")
            else:
                padding = tuple(padding)
            
            layers.append(nn.Conv2d(
                in_channels=in_channels,
                out_channels=in_channels * layer_config['groups'],
                kernel_size=tuple(layer_config['kernel_size']),
                stride=tuple(layer_config['stride']),
                padding=self._calculate_padding(padding, layer_config['kernel_size'], layer_config['stride']),
                groups=layer_config['groups']
            ))
            layers.append(self._get_activation(layer_config['activation']))

        elif layer_type == 'Conv2DTranspose':
            padding = layer_config['padding']

            # Handle padding strings
            if isinstance(padding, str):
                if padding == 'same':
                    padding = 'same'
                elif padding == 'valid':
                    padding = 0
                else:
                    raise ValueError(f"Unsupported padding type: {padding}")
            else:
                padding = tuple(padding)

            layers.append(nn.ConvTranspose2d(
                in_channels=in_channels,
                out_channels=layer_config['filters'],
                kernel_size=tuple(layer_config['kernel_size']),
                stride=tuple(layer_config['stride']),
                padding=self._calculate_padding(padding, layer_config['kernel_size'], layer_config['stride'])
            ))
            layers.append(self._get_activation(layer_config['activation']))
            
        elif layer_type == 'MaxPooling2D':
            layers.append(nn.MaxPool2d(
                kernel_size=tuple(layer_config['pool_size']),
                stride=tuple(layer_config['stride']),
                padding=self._calculate_padding('same', layer_config['pool_size'], layer_config['stride'])
            ))

        elif layer_type == 'Flatten':
            layers.append(nn.Flatten())

        elif layer_type == 'Reshape':
            layers.append(ReshapeLayer(tuple(layer_config['target_shape'])))

        elif layer_type == 'Dense':
            layers.append(nn.Linear(
                in_features=layer_config['units'],
                out_features=layer_config['units_out']
            ))
            layers.append(self._get_activation(layer_config['activation']))

        elif layer_type == 'UpSampling2D':
            layers.append(nn.Upsample(
                scale_factor=tuple(layer_config['size']),
                mode='nearest'
            ))
        
        elif layer_type == 'BatchNormalization':
            layers.append(nn.BatchNorm2d(in_channels))

        elif layer_type == 'KMeans':
            layers.append(KMeansLayer(
                num_clusters=layer_config['num_clusters'],
                num_features=in_channels
            ))

        else:
            raise ValueError(f"Unsupported layer type: {layer_type}")
        return layers

    def _calculate_padding(self, padding, kernel_size, stride):
        if padding == 'same':
            # Compute the padding needed for "same" padding
            padding = ((kernel_size[0] - 1) // 2, (kernel_size[1] - 1) // 2)
        return padding


    def _get_activation(self, activation):
        if activation == 'relu':
            return nn.ReLU()
        elif activation == 'sigmoid':
            return nn.Sigmoid()
        elif activation == 'tanh':
            return nn.Tanh()
        elif activation == 'softmax':
            return nn.Softmax(dim=1)
        else:
            raise ValueError(f"Unsupported activation function: {activation}")

    def _compile(self):
        hyperparameters = self.config['model']['hyperparameters']
        self.optimizer = optim.Adam(self.autoencoder.parameters(), lr=hyperparameters['learning_rate'])
        self.loss_function = nn.MSELoss()

    def forward(self, x):
        return self.autoencoder(x)

    def load_weights(self, weights_path):
        self.load_state_dict(torch.load(weights_path))
        print(f"Weights loaded from {weights_path}")

    def save_weights(self, path):
        torch.save(self.state_dict(), path)
        print(f"Weights saved to {path}")

# Helper class to reshape tensor in nn.Sequential
class ReshapeLayer(nn.Module):
    def __init__(self, shape):
        super(ReshapeLayer, self).__init__()
        self.shape = shape

    def forward(self, x):
        return x.view(-1, *self.shape)

if __name__ == "__main__":
    # Usage Example:
    # Assuming the YAML file is at 'autoencoder_config.yaml'
    autoencoder_model = CNNAutoencoder('model_yamls/default.yaml')
    autoencoder = autoencoder_model.autoencoder

    # Example input tensor based on input shape
    example_input = torch.randn((1, 1, 32, 24))  # Batch size of 1
    output = autoencoder(example_input)

    # Save weights
    autoencoder_model.save_weights('trained_weights.pth')
