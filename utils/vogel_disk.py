from math import *

from config import LOG_FILE_PATH

def calculate_vogel_r_array(samples: int):
    s = f'const float vogel_r_{samples}[{samples}] = float[{samples}]('
    for index in range(samples):
        r = sqrt((index + 0.5) / samples)
        comma = ',' if index < samples - 1 else ''
        s += f"\n    {r:.8f}{comma}"
    s += ');'

    # with open(LOG_FILE_PATH, 'w') as f:
    #     f.write(s)
    print(s)

calculate_vogel_r_array(64)