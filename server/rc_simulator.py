import numpy as np
import matplotlib.pyplot as plt

# Square wave generation function
def generate_square_wave(frequency, amplitude, duration, sample_rate):
    t = np.linspace(0, duration, int(sample_rate * duration), endpoint=False)
    square_wave = amplitude * np.sign(np.sin(2 * np.pi * frequency * t))
    return t, square_wave

# RC Circuit simulation function
def rc_circuit_response(square_wave, sample_rate, R, C):
    # Define the RC time constant
    tau = R * C
    dt = 1 / sample_rate
    output = np.zeros_like(square_wave)
    
    # Initial condition for the capacitor voltage
    Vc = 0
    
    for i in range(1, len(square_wave)):
        # Discrete approximation of the differential equation
        dVc = (square_wave[i] - Vc) / tau
        Vc += dVc * dt
        output[i] = Vc
        
    return output

# Parameters
frequency = 1000            # Frequency of the square wave in Hz
amplitude = 0.5            # Amplitude of the square wave in volts
duration = 0.01             # Duration of the simulation in seconds
sample_rate = 10000       # Sampling rate in Hz
R = 82000                 # Resistance in ohms
C = 2.2e-9                # Capacitance in farads
C_plate = 3e-9

# Generate square wave
t, square_wave = generate_square_wave(frequency, amplitude, duration, sample_rate)

# Simulate RC circuit response
output_wave = rc_circuit_response(square_wave, sample_rate, R, C)
cap_wave = rc_circuit_response(square_wave, sample_rate, R, C + C_plate)

# Plotting
plt.figure(figsize=(10, 6))
plt.subplot(2, 1, 1)
plt.plot(t, square_wave, label="Input Square Wave")
plt.title("Input Square Wave")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude (V)")
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(t, output_wave, label="Output Wave", color="red")
plt.plot(t, cap_wave, label="Output Wave with plate", color="blue")
plt.title("RC Circuit Output")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude (V)")
plt.grid(True)

plt.tight_layout()
plt.show()
