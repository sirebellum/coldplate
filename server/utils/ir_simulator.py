import numpy as np
import random
from multiprocessing import Pool, cpu_count
import cv2
from base64 import b64encode
import requests

class MLX90640Simulator:
    def __init__(self, rows=24, cols=32, temp_range=(20, 39)):
        self.rows = rows
        self.cols = cols
        self.temp_min, self.temp_max = temp_range

    def generate_heatmap(self):
        heatmap = np.random.uniform(self.temp_min, self.temp_min + 3, (self.rows, self.cols))
        if random.random() < 0.05:
            return heatmap, 0

        # Randomly add shapes
        num_shapes = random.choice([0, 1, 2])
        for _ in range(num_shapes):
            shape_type = random.choice(["line", "square"])
            if shape_type == "line":
                self._add_line_signature(heatmap)
            else:
                self._add_square_signature(heatmap)

        num_cats = random.choice([0, 1, 2])
        cat_type = [self._add_cat_signature(heatmap) for _ in range(num_cats)]

        return heatmap, num_cats

    def _add_line_signature(self, heatmap):
        start_x = random.randint(0, self.cols - 6)
        start_y = random.randint(0, self.rows - 6)
        direction = random.choice([(0, 1), (1, 0), (1, 1), (1, -1)])
        length = random.randint(3, 5)
        intensity_range = (27, 30)

        for i in range(length):
            x = start_x + direction[0] * i
            y = start_y + direction[1] * i
            if 0 <= x < self.cols and 0 <= y < self.rows:
                self._add_hotspot(heatmap, x, y, 1, intensity_range)

    def _add_square_signature(self, heatmap):
        start_x = random.randint(0, self.cols - 6)
        start_y = random.randint(0, self.rows - 6)
        width = random.randint(3, 5)
        height = random.randint(3, 5)
        intensity_range = (27, 30)

        for i in range(height):
            for j in range(width):
                self._add_hotspot(heatmap, start_x + j, start_y + i, 1, intensity_range)

    def _add_cat_signature(self, heatmap):
        cat_type = random.choice(["small", "large"])
        head_x = random.randint(5, self.cols - 6)
        head_y = random.randint(5, self.rows - 6)
        
        if cat_type == "small":
            head_radius = random.randint(2, 3)
            body_length = random.randint(3, 5)
            intensity_range_head = (28, 30)
            intensity_range_body = (26, 29)
        else:
            head_radius = random.randint(3, 5)
            body_length = random.randint(7, 10)
            intensity_range_head = (27, 30)
            intensity_range_body = (25, 28)

        self._add_hotspot(heatmap, head_x, head_y, head_radius, intensity_range=intensity_range_head)
        self._add_body(heatmap, head_x, head_y, head_radius, body_length, intensity_range_body)

        return cat_type

    def _add_body(self, heatmap, head_x, head_y, head_radius, body_length, intensity_range):
        direction = random.choice([(0, 1), (1, 1), (-1, 1)])
        
        for i in range(body_length):
            offset_x = direction[0] * i + random.randint(-1, 1)
            offset_y = direction[1] * (i + head_radius)
            
            if 0 <= head_x + offset_x < self.cols and 0 <= head_y + offset_y < self.rows:
                body_radius = random.randint(1, 2)
                self._add_hotspot(heatmap, head_x + offset_x, head_y + offset_y, body_radius, intensity_range)

    def _add_hotspot(self, heatmap, x, y, radius, intensity_range):
        min_intensity, max_intensity = intensity_range
        for i in range(-radius, radius + 1):
            for j in range(-radius, radius + 1):
                if 0 <= x + i < self.cols and 0 <= y + j < self.rows and (i**2 + j**2) <= radius**2:
                    heatmap[y + j, x + i] = np.random.uniform(min_intensity, max_intensity)

    def generate_batch(self, batch_size):
        with Pool(processes=16) as pool:
            data = pool.map(self._generate_heatmap_worker, range(batch_size))
        
        heatmaps = np.array([d[0] for d in data])
        cat_types = [d[1] for d in data]
        heatmaps = (heatmaps - self.temp_min) / (self.temp_max - self.temp_min)
        return heatmaps, cat_types

    def _generate_heatmap_worker(self, _):
        return self.generate_heatmap()

    def save_heatmap(self, heatmap, filename):
        # Grayscale image
        heatmap = np.uint8(heatmap * 255)
        cv2.imwrite(filename, heatmap)

# Example Usage:
if __name__ == "__main__":
    simulator = MLX90640Simulator()
    heatmaps, labels = simulator.generate_batch(42000)
    
    for idx, heatmap in enumerate(heatmaps):
        filename = f"heatmap_{idx}.png"
        heatmap = np.uint8(heatmap * 255)
        base64_image = b64encode(cv2.imencode('.png', heatmap)[1]).decode()
        json_data = {"filename": filename, "image": base64_image}
        response = requests.post("http://localhost:6969/upload_png", json=json_data)
