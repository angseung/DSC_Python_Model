"""
This script is implemented to generate pixel value in DEC for debug DSC encoder IP...
Image can be cropped to make specific image size
"""
from PIL import Image
import os
import numpy as np

wd = os.getcwd()
input_path = "/input_files"
file_path = wd + input_path

crop_width = 264
crop_height = 200

slice_width = 1
slice_height = 1

image_size = (crop_width, crop_height)
input_file_list = os.listdir(file_path)

# img_Y_list = []
# img_Cb_list = []
# img_Cr_list = []

for idx, fname in enumerate(input_file_list):
    im = Image.open(file_path + "/" + fname)
    (pic_width, pic_height) = im.size
    if ((crop_width > pic_width) or (crop_height > pic_height)):
        raise ValueError

    # image = im.load()
    crop_image = im
    # crop_image = im.crop((0, 0, crop_width, crop_height))
    crop_image = crop_image.convert("RGB")
    crop_image.save("%s_crop.jpg" % fname[:-4])
    print("PROCESSING %s FILE..." % fname)

    crop_image = np.array(crop_image)
    # np.savetxt("%s_R.txt" %fname[:-4], crop_image[:, :, 0], fmt = '%x', delimiter = '\n')
    # np.savetxt("%s_G.txt" %fname[:-4], crop_image[:, :, 1], fmt = '%x', delimiter = '\n')
    # np.savetxt("%s_B.txt" %fname[:-4], crop_image[:, :, 2], fmt = '%x', delimiter = '\n')

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

