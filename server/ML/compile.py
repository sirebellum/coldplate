import tvm
from tvm import relay
from tvm.contrib import graph_executor
import torch
from construct_model import CNNAutoencoder

# Load the model
model = CNNAutoencoder("model_yamls/default.yaml")

# Load weights
model.load_weights("trained_weights.pth")

# Define an example input shape (adjust to match your model's expected input)
input_shape = (1, 1, 32, 24)
example_input = torch.randn(input_shape)

# Convert the PyTorch model to TorchScript
try:
    scripted_model = torch.jit.trace(model.encoder, example_input).eval()
    print("TorchScript conversion successful!")
    
    # Print the TorchScript graph for debugging
    print(scripted_model.graph)
    
except Exception as e:
    print("Error during TorchScript conversion:", e)
    exit(1)

# Convert the TorchScript model to TVM Relay format
input_name = "input"
shape_dict = [(input_name, input_shape)]

# Convert the TorchScript model to Relay
mod, params = relay.frontend.from_pytorch(scripted_model, shape_dict)

# Set the target based on the installed LLVM
target = "llvm"

# Build the relay module without benchmarking
with tvm.transform.PassContext(opt_level=3):
    lib = relay.build(mod, target=target, params=params)

# Save the compiled .so, .json, and .params files
lib.export_library("model_lib.so")

# Save the graph and parameters separately
graph_json = lib.get_graph_json()
with open("model_graph.json", "w") as f:
    f.write(graph_json)
with open("model_params.params", "wb") as f:
    f.write(relay.save_param_dict(params))

# Create a graph executor
# dev = tvm.cpu()
# module = graph_executor.create(graph_json, lib, dev)

print("Model compilation successful!")
