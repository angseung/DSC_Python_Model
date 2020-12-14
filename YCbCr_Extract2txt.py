"""
This script is implemented to generate YUV pixel value in DEC for debug DSC encoder IP...
Input image must be YUV422 or YUV420 format with a *.yuv file extension.
User MUST SPECIFY image size since you file format is a RAW data format without any picture infomations.
"""
from PIL import Image
import os
import numpy as np
import matplotlib.pyplot as plt
wd = os.getcwd()
input_path = "/input_files"
yuv_path = "./input_yuv"
file_path = wd + input_path

SAMPLING_OPT = 1

### RGB444 0
## UYVY422 1
### YUV420 2

slice_width = 1
slice_height = 1

pic_size = [(720, 1280),
            (720, 1440),
            (720, 1480),
            (720, 1496),
            (720, 1520),
            (720, 1560),
            (720, 1600),
            (720, 1680),
            (1080, 1920),
            (1080, 2160),
            (1080, 2220),
            (1080, 2244),
            (1080, 2280),
            (1080, 2340),
            (1080, 2400),
            (1080, 2520)]

for psize in pic_size:

    pic_width = psize[0]
    pic_height = psize[1]
    # pic_width = 720
    # pic_height = 1280

    input_file_list = os.listdir(file_path)

    offset = int(pic_height * pic_width)
    frame_444 = np.fromfile("IMG_%d_%d_444.yuv" % (pic_width, pic_height), dtype = 'uint8', offset = 0) # planar format
    frame_422 = np.fromfile("IMG_%d_%d_422.yuv" % (pic_width, pic_height), dtype = 'uint8', offset = 0) # packed format (UYVY)
    frame_420 = np.fromfile("IMG_%d_%d_420.yuv" % (pic_width, pic_height), dtype = 'uint8', offset = 0) # packed format (YUV)

    #reshape_422 = np.reshape(frame_422, [pic_height, pic_width])

    #frame_444 = np.fromfile(yuv_path+"/IMG_720_1280_444.yuv", dtype='uint8', offset=offset*2) # planar format

    ### For 444 Mode
    reshape_444_Y = np.reshape(frame_444[0 : offset], [pic_height, pic_width])
    reshape_444_Cb = np.reshape(frame_444[offset : 2 * offset], [pic_height, pic_width])

    valid_Cb_422 = np.copy(reshape_444_Cb).astype(np.int16)
    valid_Cb_420 = np.copy(reshape_444_Cb).astype(np.int16)
    valid_Cb_422[:, 1::2] = 0
    valid_Cb_420[:, 1::2] = 0
    valid_Cb_420[1::2, :] = 0

    reshape_444_Cr = np.reshape(frame_444[2 * offset : 3 * offset], [pic_height, pic_width])

    valid_Cr_422 = np.copy(reshape_444_Cr).astype(np.int16)
    valid_Cr_420 = np.copy(reshape_444_Cr).astype(np.int16)
    valid_Cr_422[:, 1::2] = 0
    valid_Cr_420[:, 1::2] = 0
    valid_Cr_420[0::2, :] = 0

    reshape_444 = [reshape_444_Y, reshape_444_Cb, reshape_444_Cr]
    reshape_444 = np.transpose(reshape_444, [1, 2, 0])

    np.savetxt('HEX_444/IMG_%d_%d_Y_444.txt' % (pic_width, pic_height), reshape_444[:,:,0], fmt = '%x', delimiter='\n')
    np.savetxt('HEX_444/IMG_%d_%d_Cb_444.txt' % (pic_width, pic_height), reshape_444[:,:,1], fmt = '%x', delimiter='\n')
    np.savetxt('HEX_444/IMG_%d_%d_Cr_444.txt' % (pic_width, pic_height), reshape_444[:,:,2], fmt = '%x', delimiter='\n')

    ### For 422 Mode
    raw_422_Y = frame_422[1::2]
    raw_422_Cb = frame_422[::4]
    raw_422_Cr = frame_422[2::4]

    mod_422_Cb = np.zeros([offset, ], dtype = 'uint8')
    mod_422_Cr = np.zeros([offset, ], dtype = 'uint8')

    for i in range(offset):
        if (i % 2 == 0):
            mod_422_Cb[i] = raw_422_Cb[int(i // 2)]
            mod_422_Cr[i] = raw_422_Cr[int(i // 2)]

    reshape_422_Y = np.reshape(raw_422_Y, [pic_height, pic_width])
    reshape_422_Cb = np.reshape(mod_422_Cb, [pic_height, pic_width])
    reshape_422_Cr = np.reshape(mod_422_Cr, [pic_height, pic_width])

    reshape_422 = [reshape_422_Y, np.uint8(reshape_422_Cb), np.uint8(reshape_422_Cr)]
    #reshape_422 = np.transpose(reshape_422, [1, 2, 0]) # Debug

    np.savetxt('HEX_422/IMG_%d_%d_Y_422.txt' % (pic_width, pic_height), reshape_422_Y, fmt = '%x', delimiter='\n')
    np.savetxt('HEX_422/IMG_%d_%d_Cb_422.txt' % (pic_width, pic_height), reshape_422_Cb, fmt = '%x', delimiter='\n')
    np.savetxt('HEX_422/IMG_%d_%d_Cr_422.txt' % (pic_width, pic_height), reshape_422_Cr, fmt = '%x', delimiter='\n')

    is_Valid_Cb_422 = reshape_422_Cb - valid_Cb_422
    is_Valid_Cr_422 = reshape_422_Cr - valid_Cr_422

    # YUV444 = planar format
    # YUV420 = planar format
    # YUV422 = (YUYV or UYVY) = packed format

    # img_Y_list = []
    # img_Cb_list = []
    # img_Cr_list = []

    ## For 420 Mode
    raw_420_Y = frame_420[0 : offset].reshape(pic_height, pic_width)
    raw_420_Cb = frame_420[offset : (offset * 5) // 4].reshape(pic_height // 2, pic_width // 2)
    raw_420_Cr = frame_420[(offset * 5) // 4 : (offset * 6) // 4].reshape(pic_height // 2, pic_width // 2)

    mod_420_Cb_pad_h = np.zeros([pic_height, pic_width // 2], dtype = 'uint8') # Odd
    mod_420_Cr_pad_h = np.zeros([pic_height, pic_width // 2], dtype = 'uint8') # Even

    mod_420_Cb_pad_h[0::2, :] = raw_420_Cb
    mod_420_Cr_pad_h[1::2, :] = raw_420_Cr

    mod_420_Cb = np.zeros([pic_height, pic_width], dtype = 'uint8') # Odd
    mod_420_Cr = np.zeros([pic_height, pic_width], dtype = 'uint8') # Even

    mod_420_Cb[:, 0::2] = mod_420_Cb_pad_h
    mod_420_Cr[:, 0::2] = mod_420_Cr_pad_h

    # for i, j in zip(range(pic_height), range(pic_width)):
    #     if (j % 2 == 0):
    #
    #         if (i % 2 == 0): # Even Line
    #             mod_420_Cr[i, j] = raw_420_Cr[int(i / 2)]
    #         else: # (i % 2 == 1) Odd Line
    #             mod_420_Cb[i, j] = raw_420_Cb[int(i / 2)]

    np.savetxt('HEX_420/IMG_%d_%d_Y_420.txt' % (pic_width, pic_height), raw_420_Y, fmt = '%x', delimiter='\n')
    np.savetxt('HEX_420/IMG_%d_%d_Cb_420.txt' % (pic_width, pic_height), mod_420_Cb, fmt = '%x', delimiter='\n')
    np.savetxt('HEX_420/IMG_%d_%d_Cr_420.txt' % (pic_width, pic_height), mod_420_Cr, fmt = '%x', delimiter='\n')

    is_Valid_Cb_420 = mod_420_Cb - valid_Cb_420
    is_Valid_Cr_420 = mod_420_Cr - valid_Cr_420

    a = 0
    #
    # for idx, fname in enumerate(input_file_list):
    #     im = Image.open(file_path + "/" + fname)
    #     im_ycbcr = im.convert("YCbCr")
    #     im_rgb = np.array(im)
    #     im_yuv = np.array(im_ycbcr)a
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
