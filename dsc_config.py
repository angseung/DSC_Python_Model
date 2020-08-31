import os
import numpy as np
import cv2
from PIL import Image
from dsc_utils import *

################
DSC_MODE = 1
################

cfg_file_name = "test_py.cfg"
image_path = 'input_files/'
output_extension = ".ppm"
output_file_list = convert_input_images(image_path, output_extension)
START = dsc_set_cfg(DSC_MODE, cfg_file_name) & dsc_set_image_list(output_file_list)

if(START):
    start_dsc(cfg_file_name)
