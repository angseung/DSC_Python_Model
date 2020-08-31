import os
from PIL import Image
from dsc_codec import DSC_CODEC
from init_pps import *
from dsc_utils import parse_pps

sx_num = 1
sy_num = 10
################
DSC_MODE = 0
slice_width = int(1920 / sx_num)
slice_height = int(1080 / sy_num)
Bits_per_component = 8
Bits_per_pixel = 8
BLOCK_PRED_ENABLE = 1
USE_YUV_INPUT = 0
SIMPLE_422 = 0
NATIVE_422 = 0
NATIVE_420 = 0
FULL_ICH_ERR_PRECISION = 0

cfg_file_name = "test_py.cfg"
image_path = 'input_files/'
output_extension = ".ppm"
################

## MAKE CFG PARAMS TO DICT
cfg_dict = {}
cfg_dict["DSC_MODE"] = DSC_MODE
cfg_dict["slice_width"] = slice_width
cfg_dict["slice_height"] = slice_height
cfg_dict["Bits_per_component"] = Bits_per_component
cfg_dict["Bits_per_pixel"] = Bits_per_pixel
cfg_dict["BLOCK_PRED_ENABLE"] = BLOCK_PRED_ENABLE
cfg_dict["USE_YUV_INPUT"] = USE_YUV_INPUT
cfg_dict["SIMPLE_422"] = SIMPLE_422
cfg_dict["NATIVE_422"] = NATIVE_422
cfg_dict["NATIVE_420"] = NATIVE_420
cfg_dict["FULL_ICH_ERR_PRECISION"] = FULL_ICH_ERR_PRECISION

dsc = DSC_CODEC()

dsc.dsc_set_cfg(cfg_dict = cfg_dict, cfg_file_name = cfg_file_name)
dsc.convert_input_images(image_path = image_path, output_extension = output_extension)
dsc.dsc_set_image_list()
dsc.start_dsc()

### PPS Parsing Part for PPS Check
## PPS = parse_pps("2k_sample.dsc", PRINT_PPS_OPT = False)

