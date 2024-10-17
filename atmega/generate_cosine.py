import numpy as np

# Parameters
COSINE_TABLE_SIZE = 256
COSINE_MAX = 65535

# Generate cosine wave values (scaled to uint16 range)
cosine_wave = np.uint16((np.cos(2 * np.pi * np.arange(COSINE_TABLE_SIZE) / COSINE_TABLE_SIZE) + 1) * (COSINE_MAX / 2))

# Format the table as a multi-line C array for use in an AVR microcontroller with PROGMEM with 8 values per line
formatted_table = ',\n    '.join([', '.join([f'{x:5}' for x in cosine_wave[i:i+8]]) for i in range(0, COSINE_TABLE_SIZE, 8)])

formatted_c_code = f"""\
#define COSINE_TABLE_SIZE {COSINE_TABLE_SIZE}
const uint16_t cosine_wave[COSINE_TABLE_SIZE] PROGMEM = {{
    {formatted_table}
}};
"""

print(formatted_c_code)
