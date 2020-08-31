from PIL import Image
import os
import numpy as np
import matplotlib.pyplot as plt
wd = os.getcwd()
input_path = ""
yuv_path = ""
file_path = wd + input_path

slice_width = 1
slice_height = 1

pic_width = 720
pic_height = 1280

input_file_list = os.listdir(file_path)

offset = int(pic_width*pic_height)
frame_422 = np.fromfile(yuv_path+"IMG_720_1280_422.yuv", dtype='uint8', offset=0) # packed format (UYVY)
frame_444 = np.fromfile(yuv_path+"IMG_720_1280_444.yuv", dtype='uint8', offset=0) # planar format

#reshape_422 = np.reshape(frame_422, [pic_height, pic_width])

#frame_444 = np.fromfile(yuv_path+"/IMG_720_1280_444.yuv", dtype='uint8', offset=offset*2) # planar format

raw_422_Y = frame_422[1::2]
raw_422_Cb = frame_422[::4]
raw_422_Cr = frame_422[2::4]

mod_422_Cb = np.zeros([offset, ])
mod_422_Cr = np.zeros([offset, ])
for i in range(offset):
    if i % 2 == 0:
        mod_422_Cb[i] = raw_422_Cb[int(i / 2)]
        mod_422_Cr[i] = raw_422_Cr[int(i / 2)]

reshape_422_Y = np.reshape(raw_422_Y, [pic_height, pic_width])
reshape_422_Cb = np.reshape(mod_422_Cb, [pic_height, pic_width])
reshape_422_Cr = np.reshape(mod_422_Cr, [pic_height, pic_width])

reshape_422 = [reshape_422_Y, np.uint8(reshape_422_Cb), np.uint8(reshape_422_Cr)]
#reshape_422 = np.transpose(reshape_422, [1, 2, 0]) # Debug

np.savetxt('IMG_720_1280_Y.txt', reshape_422_Y, fmt = '%x', delimiter='\n')
np.savetxt('IMG_720_1280_Cb.txt', np.uint8(reshape_422_Cb), fmt = '%x', delimiter='\n')
np.savetxt('IMG_720_1280_Cr.txt', np.uint8(reshape_422_Cr), fmt = '%x', delimiter='\n')


reshape_444_Y = np.reshape(frame_444[0:offset], [pic_height, pic_width])
reshape_444_Cb = np.reshape(frame_444[offset:2*offset], [pic_height, pic_width])
reshape_444_Cr = np.reshape(frame_444[2*offset:3*offset], [pic_height, pic_width])
reshape_444 = [reshape_444_Y, reshape_444_Cb, reshape_444_Cr]
reshape_444 = np.transpose(reshape_444, [1, 2, 0])

np.savetxt('IMG_720_1280_Y.txt', reshape_444[:,:,0], fmt = '%x', delimiter='\n')
np.savetxt('IMG_720_1280_Cb.txt', reshape_444[:,:,1], fmt = '%x', delimiter='\n')
np.savetxt('IMG_720_1280_Cr.txt', reshape_444[:,:,2], fmt = '%x', delimiter='\n')
# YUV444 = planar format
# YUV420 = planar format
# YUV422 = (YUYV or UYVY) = packed format

# img_Y_list = []
# img_Cb_list = []
# img_Cr_list = []


# for idx, fname in enumerate(input_file_list):
#     im = Image.open(file_path + "/" + fname)
#     im_ycbcr = im.convert("YCbCr")
#     im_rgb = np.array(im)
#     im_yuv = np.array(im_ycbcr)
#
#     # image = im.load()
#
#     print("PROCESSING %s FILE..." % fname)
#
#     np.savetxt("%s_R.txt" %fname[:-4], im_rgb[:, :, 0], fmt = '%x', delimiter = '\n')
#     np.savetxt("%s_G.txt" %fname[:-4], im_rgb[:, :, 1], fmt = '%x', delimiter = '\n')
#     np.savetxt("%s_B.txt" %fname[:-4], im_rgb[:, :, 2], fmt = '%x', delimiter = '\n')
#
#     # orig_pixel = orig_pixel.transpose([1, 0, 2])
#     np.savetxt("%s_Y.txt" %fname[:-4], im_yuv[:, :, 0], fmt = '%x', delimiter = '\n')
#     np.savetxt("%s_Cb.txt" %fname[:-4], im_yuv[:, :, 1], fmt = '%x', delimiter = '\n')
#     np.savetxt("%s_Cr.txt" %fname[:-4], im_yuv[:, :, 2], fmt = '%x', delimiter = '\n')

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
