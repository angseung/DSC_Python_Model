import numpy as np
import os
from PIL import Image
from init_pps import *

def dsc_set_cfg(DSC_MODE = 0, cfg_file_name = "test_py.cfg"):
    cfg_line = {}
    with open("test.cfg", 'r') as cfg:
        for num, lines in enumerate(cfg):
            cfg_line[num] = lines
    #cfg_line[6][10] : DSC Mode CFG Val
    #cfg_line[48] : DSC Full_ich CFG val
    cfg_line[6] = cfg_line[6][:10] + str(DSC_MODE) + cfg_line[6][11:]
    try:
        with open(cfg_file_name, "wt") as cfg_py:
            for k, txt in cfg_line.items():
                cfg_py.write(txt)
        print("Setting "+ '"' + cfg_file_name + '"' + " file...\n")
        return 1

    except:
        return None

def convert_input_images(image_path = 'input_files/', output_extension = "ppm"):
    print("Converting Input Images to " + '"' + output_extension.upper() + '"' + " Extension\n")
    input_file_list = os.listdir(image_path)
    output_file_list = []
    try:
        for fname in input_file_list:
            name_index = fname.find(".")
            im = Image.open(image_path + fname)
            fname_ = fname[: name_index] + output_extension
            output_file_list.append(fname_)
            im.save(os.getcwd() + "/" + fname_)

        return output_file_list

    except:
        return None

def dsc_set_image_list(test_image_list = None):
    try:
        with open("test_list.txt", "w") as f:
            for f_name in test_image_list:
                f.write(f_name + "\n")
        print("Writing Input File List\n")
        return 1

    except:
        return None

def start_dsc(cfg_file_name = "test_py.cfg"):
    print("Starting DSC Model\n\n")
    dsc_path = os.getcwd()
    os.system("cd {}".format(dsc_path))
    os.system("DSC -F " + cfg_file_name)

def parse_pps(path = "w1.dsc", PRINT_PPS_OPT = False):
    PPS = initPps()

    with open(path, "rb") as f:
        if (f.read(4) != b'DSCF'):  # Magic Number
            raise ValueError("This file is not a DSC file.")

        tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
        PPS.dsc_version_major = (tmp >> 4) & 0xf
        PPS.dsc_version_minor = tmp & 0xf

        PPS.pps_identifier = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)  # PPS Identifier
        PPS.RESERVED = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)  # RESERVED 8bits

        tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
        PPS.bits_per_component = (tmp >> 4) & 0xf
        PPS.line_buf_depth = tmp & 0xf

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.block_pred_enable = (tmp >> 13) & 0b1
        PPS.convert_rgb = (tmp >> 12) & 0b1
        PPS.simple_422 = (tmp >> 11) & 0b1
        PPS.vbr_enable = (tmp >> 10) & 0b1
        PPS.bits_per_pixel = (tmp & 0b0000001111111111)

        PPS.pic_height = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.pic_width = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.slice_height = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.slice_width = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.chunk_size = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.initial_xmit_delay = (tmp & 0b0000001111111111)

        PPS.initial_dec_delay = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.initial_scale_value = (tmp & 0b0000000000111111)

        PPS.scale_increment_interval = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.scale_decrement_interval = (tmp & 0xfff)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.first_line_bpg_ofs = tmp & 0b0000000000011111

        PPS.nfl_bpg_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.slice_bpg_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.initlal_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.final_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.flatness_min_qp = (tmp & 0b0001111100000000) >> 8
        PPS.RESERVED = 0x0
        PPS.flatness_max_qp = (tmp & 0b0000000000011111)

        PPS.rc_model_size = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.rc_edge_factor = (tmp & 0b0000111100000000) >> 8
        PPS.RESERVED = 0x0
        PPS.rc_quant_icnr_limit0 = (tmp & 0b0000000000011111)

        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.rc_quant_icnr_limit1 = (tmp & 0b0001111100000000) >> 8
        PPS.rc_tgt_offset_hi = (tmp & 0b0000000011110000) >> 4
        PPS.rc_tgt_offset_lo = (tmp & 0b0000000000001111)

        # rc_buf_thresh, (14, ) shape ndarray, with 6 bit fractional bits
        PPS.rc_buf_thresh = np.array([
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6,
            int.from_bytes(f.read(1), byteorder = 'big', signed = 0) << 6
        ])

        # rc_range_parameters, (15, 3) shape ndarray
        rc_range_parameters = np.zeros(shape = (15, 3), dtype = np.int32)
        for i in range(rc_range_parameters.shape[0]):
            tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
            rc_range_parameters[i, :] = [tmp >> 11, (tmp >> 6) & 0b11111, tmp & 0b111111]
            if rc_range_parameters[i, 2] >= 32:
                rc_range_parameters[i, 2] -= 64

        # insert ndarray to dict PPS
        PPS.rc_range_parameters = rc_range_parameters

        tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.native_420 = (tmp >> 1) & 0b1
        PPS.native_422 = tmp & 0b1

        tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
        PPS.RESERVED = 0x0
        PPS.second_line_bpg_offset = tmp & 0b11111

        PPS.nsl_bpg_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        PPS.second_line_ofs_adj = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

        PPS.RESERVED = int.from_bytes(f.read(34), byteorder = 'big', signed = 0)

    if PRINT_PPS_OPT:
        for key, val in PPS.items():
            print("PPS : %s, Value : " %key, val)

    return PPS