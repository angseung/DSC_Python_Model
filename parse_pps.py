import os
import numpy as np

file_path = os.getcwd()
PRINT_PPS_OPT = 1

with open("sample_01.dsc", "rb") as f:
    if(f.read(4) != b'DSCF'): # Magic Number
        exit(-1)

    tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
    PPS["dsc_version_major"] = (tmp >> 4) & 0xf
    PPS["dsc_version_minor"] = tmp & 0xf

    PPS["pps_identifier"] = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)  # PPS Identifier
    PPS["RESERVED"] = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)  # RESERVED 8bits

    tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
    PPS["bits_per_component"] = (tmp >> 4) & 0xf
    PPS["linebuf_depth"] = tmp & 0xf

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    RESERVED = (tmp >> 14) & 0xf
    blockpred_enable = (tmp >> 13) & 0x1
    convert_rgb = (tmp >> 12) & 0x1
    simple_422 = (tmp >> 11) & 0x1
    vbr_enable = (tmp >> 10) & 0x1
    bits_per_pixel = (tmp & 0b0000001111111111)

    pic_height = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    pic_width = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    slice_height = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    slice_width = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    chunk_size = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    RESERVED = tmp >> 10
    initial_xmit_delay = (tmp & 0b0000001111111111)

    initial_dec_delay = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    initial_scale_value = (tmp & 0b0000000000111111)

    scale_increment_interval = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    RESERVED = tmp >> 12
    scale_decrement_interval = (tmp & 0xfff)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    RESERVED = tmp >> 5
    first_line_bpg_ofs = tmp & 0b0000000000011111

    nfl_bpg_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    slice_bpg_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    initlal_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    final_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    flatness_min_qp = (tmp & 0b0001111100000000) >> 8
    flatness_max_qp = (tmp & 0b0000000000011111)

    rc_model_size = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    rc_edge_factor = (tmp & 0b0000111100000000) >> 8
    rc_quant_icnr_limit0 = (tmp & 0b0000000000011111)

    tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    rc_quant_icnr_limit1 = (tmp & 0b0001111100000000) >> 8
    rc_tgt_offset_hi = (tmp & 0b0000000011110000) >> 4
    rc_tgt_offset_lo = (tmp & 0b0000000000001111)

    # rc_buf_thresh, (14, ) shape ndarray
    rc_buf_thresh = np.array([
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0),
        int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
    ])

    # rc_range_parameters, (15, 3) shape ndarray
    rc_range_parameters = np.zeros(shape = (15, 3))
    for i in range(rc_range_parameters.shape[0]):
        tmp = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
        rc_range_parameters[i, :] = [tmp >> 11, (tmp >> 6) & 0b11111, tmp & 0b111111]
        if rc_range_parameters[i, 2] >= 32:
            rc_range_parameters[i, 2] -= 64

    tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
    native_420 = (tmp >> 1) & 0b1
    native_422 = tmp & 0b1

    tmp = int.from_bytes(f.read(1), byteorder = 'big', signed = 0)
    second_line_bpg_offset = tmp & 0b11111

    nsl_bpg_offset = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)
    second_line_ofs_adj = int.from_bytes(f.read(2), byteorder = 'big', signed = 0)

    RESERVED = int.from_bytes(f.read(31), byteorder = 'big', signed = 0)
