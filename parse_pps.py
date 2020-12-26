"""
This script can be used to parse pps information from dsc file header.
"""

import os
import numpy as np
from init_pps import initPps
from PPS_readnwrite import parse_pps

file_name = "NEW_image_half.dsc"
PRINT_PPS_OPT = 1
PPS = initPps()
PPS = parse_pps(file_name, 0)