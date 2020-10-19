import numpy as np
import pandas as pd

def reconstruct_dsc_data(file_name = None, pic_width = 1920, pic_height = 1080,
                         num_h_slice = 1, num_v_slice = 1,
                         byte_offset = 132,
                         LANE_DISTRIBUTE_OPT = False):

    slice_width = pic_width // num_h_slice
    slice_height = pic_height // num_v_slice
    slice_size = slice_width * slice_height

    dsc_data_from_file = np.fromfile(file_name, np.uint8, -1, '', 0) ## in case of header removed file...
    # dsc_data_from_file = np.fromfile(file_name, np.uint8, -1, '', 132) ## Pixel data comes from 132th bytes in original dsc file...
    dsc_data_orig = dsc_data_from_file.reshape((num_h_slice * slice_height, slice_width))

    hss = np.zeros((slice_height, 576), dtype = np.uint8)
    hss[:, 0] = 0x21
    hss[:, 3] = 0x12
    hss[:, 4] = 0x19
    hss[:, 5] = 0x36
    hss[:, 6] = 0x02
    hss[:, 7] = 0x18
    hss[:, -2 :] = 0x99

    head_0 = np.zeros((slice_height, 4), dtype = np.uint8)
    head_0[:, 0] = 0x0b
    head_0[:, 1] = 0xe0
    head_0[:, 2] = 0x01
    head_0[:, 3] = 0x1c

    slice_0 = dsc_data_from_file[0 : slice_size].reshape([slice_height, slice_width])

    head_1 = np.zeros((slice_height, 6), dtype = np.uint8)
    head_1[:, 0] = 0x99
    head_1[:, 1] = 0x99
    head_1[:, 2] = 0x0b
    head_1[:, 3] = 0x01
    head_1[:, 4] = 0xe0
    head_1[:, 5] = 0x99

    slice_1 = dsc_data_from_file[slice_size : slice_size * 2].reshape([slice_height, slice_width])

    slice_2 = dsc_data_from_file[slice_size * 2 : slice_size * 3].reshape([slice_height, slice_width])

    slice_3 = dsc_data_from_file[slice_size * 3 : slice_size * 4].reshape([slice_height, slice_width])

    head_2 = np.zeros((slice_height, 6), dtype = np.uint8)
    head_2[:, 0] = 0x99
    head_2[:, 1] = 0x99
    head_2[:, 2] = 0x19
    head_2[:, 3] = 0xea
    head_2[:, 5] = 0x0c

    blank = np.zeros((slice_height, 236), dtype = np.uint8)
    blank[:, -2 :] = 0x99

    dsc_recon = np.hstack([
        hss, # HSS Header,
        head_0, # Compressed Pixel packet header
        slice_0,
        head_1, # 2 Byte CRC + Compressed Pixel packet header
        slice_1,
        head_1, # 2 Byte CRC + Compressed Pixel packet header
        slice_2,
        head_1, # 2 Byte CRC + Compressed Pixel packet header
        slice_3,
        head_2, # 2 Byte CRC + Blanking packet header
        blank # Blanking packet payload
    ])

    if LANE_DISTRIBUTE_OPT:
        ## Distribute data onto data lanes...
        lane_idx = np.array(range(dsc_recon.shape[1]), dtype =  np.uint32)
        lane0 = lane_idx[0 :: 4]
        lane1 = lane_idx[1 :: 4]
        lane2 = lane_idx[2 :: 4]
        lane3 = lane_idx[3 :: 4]

        for i in range(pic_height):
            temp = dsc_recon[i, :]
            temp2 = np.vstack([
                temp[lane0],
                temp[lane1],
                temp[lane2],
                temp[lane3]
            ])
            if (i == 0):
                dsc_data_lane = temp2
            else:
                dsc_data_lane = np.hstack([dsc_data_lane, temp2])

        ## Save data to csv file in decimal value...
        dsc_data_lane = dsc_data_lane.transpose([1, 0])
        DF = pd.DataFrame(dsc_data_lane)
        DF.to_csv("DSC_%s.csv" %file_name[: -4])

# file_name = "RGBW_image_4_1.dsc"
file_name = "RED_image_4_1.dsc"

reconstruct_dsc_data(file_name = file_name,
                     pic_width = 1920, pic_height = 1080,
                     num_h_slice = 4, num_v_slice = 1,
                     byte_offset = 0,
                     LANE_DISTRIBUTE_OPT = True)