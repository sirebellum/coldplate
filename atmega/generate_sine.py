import numpy as np

# Parameters
SINE_TABLE_SIZE = 256
SINE_MAX = 65535

# Generate sine wave values (scaled to uint16 range)
sine_wave = np.uint16((np.sin(2 * np.pi * np.arange(SINE_TABLE_SIZE) / SINE_TABLE_SIZE) + 1) * (SINE_MAX / 2))

# Format the table as a multi-line C array for use in an AVR microcontroller with PROGMEM with 8 values per line
formatted_table = ',\n    '.join([', '.join([f'{x:5}' for x in sine_wave[i:i+8]]) for i in range(0, SINE_TABLE_SIZE, 8)])

formatted_c_code = f"""\
#define SINE_TABLE_SIZE {SINE_TABLE_SIZE}
const uint16_t sine_wave[SINE_TABLE_SIZE] PROGMEM = {{
    {formatted_table}
}};
"""

print(formatted_c_code)
