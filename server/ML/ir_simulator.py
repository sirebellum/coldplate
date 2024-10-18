import numpy as np
import random

class MLX90640Simulator:
    def __init__(self, rows=24, cols=32, temp_range=(15, 30)):
        """
        Initialize the simulator with grid dimensions and temperature range.
        
        Args:
            rows (int): Number of rows in the simulated heatmap (default 24).
            cols (int): Number of columns in the simulated heatmap (default 32).
            temp_range (tuple): Min and max temperature values (in degrees Celsius).
        """
        self.rows = rows
        self.cols = cols
        self.temp_min, self.temp_max = temp_range

    def generate_heatmap(self):
        """
        Generate a single heatmap with patterns resembling cat heat signatures.
        
        Returns:
            numpy.ndarray: 2D array representing temperature values.
        """
        # Start with room temperature background
        heatmap = np.random.uniform(self.temp_min, self.temp_min + 3, (self.rows, self.cols))
        
        # Randomly decide if there will be one or two cats
        num_cats = random.choice([1, 2])
        
        # Add cat heat signatures
        for _ in range(num_cats):
            self._add_cat_signature(heatmap)
            
        return heatmap

    def _add_cat_signature(self, heatmap):
        """
        Modify the heatmap to include a cat-like heat signature.
        
        Args:
            heatmap (numpy.ndarray): 2D array to be modified.
        """
        # Randomly choose small or large cat
        cat_type = random.choice(["small", "large"])
        
        # Position the "cat" at a random spot on the grid, avoiding the edges
        head_x = random.randint(5, self.cols - 6)
        head_y = random.randint(5, self.rows - 6)
        
        if cat_type == "small":
            # Smaller cat: tighter cluster of heat, shorter "body"
            head_radius = random.randint(2, 3)
            body_length = random.randint(3, 5)
            intensity_range_head = (28, 30)
            intensity_range_body = (26, 29)
        else:
            # Larger cat: wider cluster for head, longer "body"
            head_radius = random.randint(3, 5)
            body_length = random.randint(7, 10)
            intensity_range_head = (27, 30)
            intensity_range_body = (25, 28)

        # Create the head as a hotspot
        self._add_hotspot(heatmap, head_x, head_y, head_radius, intensity_range=intensity_range_head)
        
        # Add the body extending from the head position
        self._add_body(heatmap, head_x, head_y, head_radius, body_length, intensity_range_body)

    def _add_body(self, heatmap, head_x, head_y, head_radius, body_length, intensity_range):
        """
        Add a body-like shape extending from the head hotspot.
        
        Args:
            heatmap (numpy.ndarray): The heatmap to modify.
            head_x (int): The x-coordinate of the head center.
            head_y (int): The y-coordinate of the head center.
            head_radius (int): The radius of the head.
            body_length (int): Length of the body in "segments".
            intensity_range (tuple): Min and max intensity for the body segments.
        """
        # Choose a general direction for the body to extend (down, right-down, or left-down)
        direction = random.choice([(0, 1), (1, 1), (-1, 1)])
        
        for i in range(body_length):
            offset_x = direction[0] * i + random.randint(-1, 1)
            offset_y = direction[1] * (i + head_radius)
            
            if 0 <= head_x + offset_x < self.cols and 0 <= head_y + offset_y < self.rows:
                body_radius = random.randint(1, 2)
                self._add_hotspot(heatmap, head_x + offset_x, head_y + offset_y, body_radius, intensity_range)

    def _add_hotspot(self, heatmap, x, y, radius, intensity_range):
        """
        Add a circular hotspot to the heatmap to simulate a heated area.
        
        Args:
            heatmap (numpy.ndarray): The heatmap to modify.
            x (int): The x-coordinate of the hotspot center.
            y (int): The y-coordinate of the hotspot center.
            radius (int): The radius of the hotspot.
            intensity_range (tuple): Min and max intensity for the hotspot.
        """
        min_intensity, max_intensity = intensity_range
        for i in range(-radius, radius + 1):
            for j in range(-radius, radius + 1):
                if 0 <= x + i < self.cols and 0 <= y + j < self.rows and (i**2 + j**2) <= radius**2:
                    heatmap[y + j, x + i] = np.random.uniform(min_intensity, max_intensity)

    def generate_batch(self, batch_size):
        """
        Generate a batch of heatmaps.
        
        Args:
            batch_size (int): Number of heatmaps to generate.
        
        Returns:
            numpy.ndarray: 3D array (batch_size, rows, cols).
        """
        heatmaps = np.array([self.generate_heatmap() for _ in range(batch_size)])
        # Scale between 0 and 1
        return (heatmaps - self.temp_min) / (self.temp_max - self.temp_min)

    def save_heatmap(self, heatmap, filename):
        """
        Save a heatmap to an image file.
        
        Args:
            heatmap (numpy.ndarray): 2D array representing the heatmap.
            filename (str): Name of the output file.
        """
        import matplotlib.pyplot as plt
        plt.imshow(heatmap, cmap='hot', interpolation='nearest')
        plt.axis('off')
        plt.savefig(filename)


# Example Usage:
if __name__ == "__main__":
    simulator = MLX90640Simulator()
    batch = simulator.generate_batch(5)
    print(batch.shape)  # Should print (5, 24, 32)

    # Write output to image on disk
    import matplotlib.pyplot as plt
    fig, axes = plt.subplots(1, 5, figsize=(20, 5))
    for i in range(5):
        axes[i].imshow(batch[i], cmap='hot', interpolation='nearest')
        axes[i].axis('off')
        axes[i].set_title(f"Sample {i + 1}")
    plt.tight_layout()
    plt.savefig("heatmap_samples.png")
    