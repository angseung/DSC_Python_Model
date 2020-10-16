import numpy as np
import pandas as pd

file_name = "RGBW_image_4_1.dsc"

pic_width = 1920
pic_height = 1080

num_h_slice = 4
num_v_slice = 1

slice_width = pic_width // num_h_slice
slice_height = pic_height // num_v_slice

#### for debug opt
dsc_data_from_file = np.fromfile(file_name, np.uint8, -1, '', 0)
dsc_data_orig = dsc_data_from_file.reshape((num_h_slice * slice_height, slice_width))
# dsc_data_orig = dsc_data_orig.transpose([1, 0])

# dsc_data_orig = np.zeros((1080, 1920), dtype = np.uint8)
# dsc_data_orig[0 : 1080, :] = 1
# dsc_data_orig[1080 : 2160, :] = 2
# dsc_data_orig[2160 : 3240, :] = 3
# dsc_data_orig[3240 : 4320, :] = 4
# dsc_data_orig = dsc_data_orig.flatten()

# dsc_data_recon = np.zeros((slice_height * num_h_slice, slice_width), dtype = np.uint8)
# dsc_data_recon[0 : 1080, :] = dsc_data_orig[:, 0 : 480]
# dsc_data_recon[1080 : 2160, :] = dsc_data_orig[:, 480 : 960]
# dsc_data_recon[2160 : 3240, :] = dsc_data_orig[:, 960 : 1440]
# dsc_data_recon[3240 : 4320, :] = dsc_data_orig[:, 1440 : ]

dsc_data_recon = np.zeros(dsc_data_orig.shape, dtype = np.uint8)

for i in range(slice_height):
    curr_idx = np.array(range(i, slice_height * num_h_slice, slice_height))
    temp = dsc_data_orig[curr_idx, :]
    temp = temp.flatten()

    if (i == 0):
        dsc_data_recon = temp.copy()
    else:
        dsc_data_recon = np.vstack((dsc_data_recon, temp))

lane_idx = np.array(range(pic_width), dtype =  np.uint32)
lane0 = lane_idx[0 :: 4]
lane1 = lane_idx[1 :: 4]
lane2 = lane_idx[2 :: 4]
lane3 = lane_idx[3 :: 4]

for i in range(pic_height):
    temp = dsc_data_recon[i, :]
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

# dsc_data_lane_hex = np.zeros(dsc_data_lane.shape)

# for i in range(dsc_data_lane.shape[0]):
#     for j in range(dsc_data_lane.shape[1]):
#         val = dsc_data_lane[i, j].item()
#         dsc_data_lane_hex[i, j] = hex(val)

DF = pd.DataFrame(dsc_data_lane.transpose([1, 0]))
DF.to_csv("DSC_RGBW_image_4_1.csv")

# dsc_data = np.hstack([
#     dsc_data_recon[0 : 1080, :],
#     dsc_data_recon[1080 : 2160, :],
#     dsc_data_recon[2160 : 3240, :],
#     dsc_data_recon[3240 : 4320, :],
# ])

# index = np.array(range(num_h_slice * slice_height), dtype = np.uint32)
# index_0 = index[0 :: 4]
# index_1 = index[1 :: 4]
# index_2 = index[2 :: 4]
# index_3 = index[3 :: 4]
#
# dsc_data[0 : 1080, :] = dsc_data_recon[index_0, :]
# dsc_data[1080 : 2160, :] = dsc_data_recon[index_1, :]
# dsc_data[2160 : 3240, :] = dsc_data_recon[index_2, :]
# dsc_data[3240 : 4320, :] = dsc_data_recon[index_3, :]

# if (num_v_slice == 1):
#     # dsc_data_orig = np.fromfile(file_name, np.uint8, -1, '', 132)
#     # dsc_data_orig = dsc_data_orig.reshape(slice_width * slice_height, num_h_slice)
#     dsc_data_recon = np.zeros((slice_height * num_h_slice, slice_width), dtype = np.uint8)
#
#     dsc_data_slice = np.zeros((slice_height, slice_width, num_h_slice), dtype = np.uint8)
#
#     for i in range(num_h_slice):
#         temp = dsc_data_orig[i * (slice_height * slice_width) : (i + 1) * (slice_height * slice_width)]
#         temp = temp.reshape(slice_height, slice_width)
#
#         dsc_data_slice[:, :, i] = temp
#
#     curr_row = 0
#
#     ## Byte Reorganize...
#     for i in range(slice_height):
#         for j in range(num_h_slice):
#             dsc_data_recon[curr_row, :] = dsc_data_slice[i, :, j]
#             curr_row = curr_row + 1
#
#     # dsc_data_recon = dsc_data_recon.flatten()

# else:
#     raise NotImplementedError("A case of horizontal multiple slice is not implemented yet...")