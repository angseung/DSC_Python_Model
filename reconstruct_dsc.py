import numpy as np

file_name = "w1.dsc"
pic_width = 1920
pic_height = 1080

num_h_slice = 4
num_v_slice = 1

slice_width = pic_width // num_h_slice
slice_height = pic_height // num_v_slice

dsc_data_orig = np.fromfile(file_name, np.uint8, -1, '', 132)
dsc_data_orig = dsc_data_orig.reshape(slice_width * slice_height, num_h_slice)
