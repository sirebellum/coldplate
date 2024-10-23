import tvm
from tvm import relay
from tvm.contrib import graph_executor
import torch
from construct_model import CNNAutoencoder
from tvm.contrib import cc

# Load the quantized model
model = CNNAutoencoder("model_yamls/depthwise.yaml")
model.load_weights("trained_weights.pth")

# Define an example input shape (adjust to match your model's expected input)
input_shape = (1, 1, 32, 24)
example_input = torch.randn(input_shape)

# Convert the model to TorchScript format
model = torch.nn.Sequential(model.encoder, model.kmeans)
scripted_model = torch.jit.trace(model, example_input)

# Convert the TorchScript model to TVM Relay format
input_name = "input"
shape_list = [(input_name, input_shape)]

# Convert the TorchScript model to Relay, handle quantized operations
mod, params = relay.frontend.from_pytorch(scripted_model, shape_list)

# Set the target based on the installed LLVM, adjust for banana pi
target = "llvm -mtriple=arm-linux-gnueabihf"
target += " -mcpu=cortex-a7"

# Build the relay module with optimizations and other passes
with tvm.transform.PassContext(opt_level=3, config={
    "tir.disable_vectorize": False,       # Enable vectorization
    "relay.backend.use_auto_scheduler": True,  # Use auto-scheduler if available
}):
    lib = relay.build(mod, target=target, params=params)

# Save shared object
sysroot = "/Users/josh/sysroot/armbian"
options = [
    "--sysroot=" + sysroot,
    "-fuse-ld=lld", "--target=arm-linux-gnueabihf",
    ]
lib.export_library("model_lib.so", options=options)

# Save the graph and parameters separately
graph_json = lib.get_graph_json()
with open("model_graph.json", "w") as f:
    f.write(graph_json)
with open("model_params.params", "wb") as f:
    f.write(relay.save_param_dict(params))

# Optional: Create a graph executor
# dev = tvm.cpu()
# module = graph_executor.create(graph_json, lib, dev)
