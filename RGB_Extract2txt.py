"""
This script is implemented to generate RGB pixel value in DEC for debug DSC encoder IP...
"""
from PIL import Image
import os
import numpy as np

wd = os.getcwd();
input_path = "/input_files"
file_path = wd + input_path

slice_width = 1
slice_height = 1

input_file_list = os.listdir(file_path)

# img_Y_list = []
# img_Cb_list = []
# img_Cr_list = []

for idx, fname in enumerate(input_file_list):
    im = Image.open(file_path + "/" + fname)
    im_ycbcr = im.convert("YCbCr")
    im_rgb = np.array(im)
    im_yuv = np.array(im_ycbcr)

    # image = im.load()

    print("PROCESSING %s FILE..." % fname)

    np.savetxt("%s_R.txt" %fname[:-4], im_rgb[:, :, 0], fmt = '%x', delimiter = '\n')
    np.savetxt("%s_G.txt" %fname[:-4], im_rgb[:, :, 1], fmt = '%x', delimiter = '\n')
    np.savetxt("%s_B.txt" %fname[:-4], im_rgb[:, :, 2], fmt = '%x', delimiter = '\n')

    # orig_pixel = orig_pixel.transpose([1, 0, 2])
    # np.savetxt("%s_Y.txt" %fname[:-4], im_yuv[:, :, 0], fmt = '%x', delimiter = '\n')
    # np.savetxt("%s_Cb.txt" %fname[:-4], im_yuv[:, :, 1], fmt = '%x', delimiter = '\n')
    # np.savetxt("%s_Cr.txt" %fname[:-4], im_yuv[:, :, 2], fmt = '%x', delimiter = '\n')

    # for i in range(im_ycbcr.height):
    #     for j in range(im_ycbcr.width):
    #         img_Y_list.append(im_pixel[i, j, 0])
    #         img_Cb_list.append(im_pixel[i, j, 1])
    #         img_Cr_list.append(im_pixel[i, j, 2])

            # print("Y : %3d, Cg : %3d, Cr : %3d\n" %(image[i, j][0], image[i, j][1], image[i, j][2]))

    # with open(("%s_Y.txt" %fname), "w") as f:
    #     for val in img_Y_list:
    #         f.write(str(hex(val)) + ',\n')
    #
    # with open(("%s_Cb.txt" %fname), "w") as f:
    #     for val in img_Cb_list:
    #         f.write(str(hex(val)) + ',\n')
    #
    # with open(("%s_Cr.txt" %fname), "w") as f:
    #     for val in img_Cr_list:
    #         f.write(str(hex(val)) + ',\n')

    # img_Y_list = []
    # img_Cb_list = []
    # img_Cr_list = []

