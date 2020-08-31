import numpy as np
import cv2
from PIL import Image

path = 'C:/Users/ISW/Downloads/image1920x1080 z0 za zb zc zd/'
input_extension = "bmp"
output_extension = "ppm"

for i in range(17):
    input_file_name = path + "z" + str(i) + "." + input_extension
    im = Image.open(input_file_name)
    input_file_name = path + "z" + str(i) + "." + output_extension
    im.save(path + "z" + str(i) + "." + output_extension)
