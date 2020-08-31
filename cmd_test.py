import os
import numpy as np
import cv2
from PIL import Image

image_path = 'input_files/'
input_extension = ".bmp"
output_extension = ".ppm"
input_file_list = os.listdir(image_path)
output_file_list = []

for fname in input_file_list:
    name_index = fname.find(".")
    im = Image.open(image_path + fname)
    fname_ = fname[: name_index] + output_extension
    output_file_list.append(fname_)
    im.save(os.getcwd() + "/" + fname_)

with open("test_list.txt", "w") as f:
    for f_name in output_file_list:
        f.write(f_name + "\n")

# START DSC Model
dsc_path = os.getcwd()
os.system("cd {}".format(dsc_path))
os.system("DSC -F test.cfg")