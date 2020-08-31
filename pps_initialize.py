import numpy as np
import os
from math import ceil

def init_pps(pic_width = 1920, pic_height = 1080):

    PRINT_DEBUG_OPT = False
    PPS= {}
    PPS_CFG = {}
    rc_range_parameters_table = [
    np.array([
        [0, 4, 2],
        [0, 4, 0],
        [1, 5, 0],
        [1, 6, -2],
        [3, 7, -4],
        [3, 7, -6],
        [3, 7, -8],
        [3, 8, -8],
        [3, 9, -8],
        [3, 10, -10],
        [5, 10, -10],
        [5, 11, -12],
        [5, 11, -12],
        [9, 12, -12],
        [12, 13, -12]
    ]),
    np.array([
        [0, 8, 2],
        [4, 8, 0],
        [5, 9, 0],
        [5, 10, -2],
        [7, 11, -4],
        [7, 11, -6],
        [7, 11, -8],
        [7, 12, -8],
        [7, 13, -8],
        [7, 14, -10],
        [9, 14, -10],
        [9, 15, -12],
        [9, 15, -12],
        [13, 16, -12],
        [16, 17, -12]
    ]),
    np.array([
        [0, 12, 2],
        [4, 12, 0],
        [9, 13, 0],
        [9, 14, -2],
        [11, 15, -4],
        [11, 15, -6],
        [11, 15, -8],
        [11, 16, -8],
        [11, 17, -8],
        [11, 18, -10],
        [13, 18, -10],
        [13, 19, -12],
        [13, 19, -12],
        [17, 20, -12],
        [20, 21, -12]
    ]),
    np.array([
        [0, 2, 0],
        [0, 4, 0],
        [1, 5, 0],
        [1, 6, -2],
        [3, 7, -4],
        [3, 7, -6],
        [3, 7, -8],
        [3, 8, -8],
        [3, 8, -8],
        [3, 9, -10],
        [5, 9, -10],
        [5, 9, -12],
        [5, 9, -12],
        [7, 10, -12],
        [10, 11, -12]
    ]),
    np.array([
        [0, 2, 0],
        [2, 5, 0],
        [3, 7, 0],
        [4, 8, -2],
        [6, 9, -4],
        [7, 10, -6],
        [7, 11, -8],
        [7, 12, -8],
        [7, 12, -8],
        [7, 13, -10],
        [9, 13, -10],
        [9, 13, -12],
        [9, 13, -12],
        [11, 14, -12],
        [14, 15, -12]
    ]),
    np.array([
        [0, 6, 2],
        [4, 9, 2],
        [7, 11, 0],
        [8, 12, -2],
        [10, 13, -4],
        [11, 14, -6],
        [11, 15, -8],
        [11, 16, -8],
        [11, 16, -8],
        [11, 17, -10],
        [13, 17, -10],
        [13, 17, -12],
        [13, 17, -12],
        [15, 18, -12],
        [18, 19, -12]
    ])]

    initial_xmit_delay_table = [512, 512, 512, 341, 341, 341]
    first_lie_bpg_offset_table = [15, 15, 15, 15, 15, 15]
    initial_offset_table = [6144, 6144, 6144, 2048, 2048, 2048]
    flatness_min_qp_table = [3, 7, 11, 3, 7, 11]
    flatness_max_qp_table = [12, 16, 20, 12, 16, 20]
    rc_quant_incr_limit_table = [11, 15, 19, 11, 15, 19]

    ## Setting Encoder Configuration HERE...

    dsc_version_major = 1
    dsc_version_minor = 2
    pps_identifier = 0
    bits_per_component = 8
    line_buf_depth = 0
    block_pred_enable = 1
    convert_rgb = 0
    simple_422 = 0
    native_420 = 0
    native_422 = 0
    vbr_enable = 0
    bits_per_pixel = 8 << 4 # 4-bits Fractional

    ## Table Select Param
    ## table_sel
    # 8bpp_8bpc : 0
    # 8bpp_10bpc : 1
    # 8bpp_12bpc : 2
    # 12bpp_8bpc : 3
    # 12bpp_10bpc : 4
    # 12bpp_12bpc : 5

    if bits_per_pixel == 8 << 4:
        if bits_per_component == 8:
            table_sel = 0
        elif bits_per_component == 10:
            table_sel = 1
        elif bits_per_component == 12:
            table_sel = 2
        muxWordSize = 48

    elif bits_per_pixel == 12 << 4:
        if bits_per_component == 8:
            table_sel = 3
        elif bits_per_component == 10:
            table_sel = 4
        elif bits_per_component == 12:
            table_sel = 5
        muxWordSize = 64

    pic_height = pic_height
    pic_width = pic_width
    slice_height = 108
    slice_width = 480
    chunk_size = 480
    rc_model_size = 8192
    rc_edge_factor = 6
    rc_tgt_offset_hi = 3
    rc_tgt_offset_lo = 3
    initial_xmit_delay = initial_xmit_delay_table[table_sel]
    numSsps = 4 if native_422 else 3
    initial_fullness_offset = 6144

    maxSeSize_Y = 4 * bits_per_component + 4
    maxSeSize_C = 4 * bits_per_component

    if convert_rgb:
        numExtraMuxBits = numSsps * (muxWordSize + (4 * bits_per_component + 4) - 2)
    elif not native_422:
        numExtraMuxBits = numSsps * muxWordSize + maxSeSize_Y + 2 * maxSeSize_C - 2
    else:
        numExtraMuxBits = numSsps * muxWordSize + maxSeSize_Y + 3 * maxSeSize_C - 2

    slicebit = 8 * chunk_size * slice_height

    while((numExtraMuxBits > 0) and ((slicebit - numExtraMuxBits) % muxWordSize)):
        numExtraMuxBits -= 1

    pixelsPerGroup = 3
    groupsPerLine = int(slice_width / pixelsPerGroup)
    groupsTotal = groupsPerLine * slice_height

    ######## FOR CALCULATED PPSs...

    #initial_dec_delay = []
    #initial_scale_value = []
    #scale_increment_interval = []
    #scale_decrement_interval = []
    first_line_bpg_offset = first_lie_bpg_offset_table[table_sel]
    nfl_bpg_offset = int(ceil(float((first_line_bpg_offset << 11) / (slice_height - 1))))
    #slice_bpg_offset = []
    initial_offset = initial_offset_table[table_sel]
    final_offset = rc_model_size - ((initial_xmit_delay * bits_per_pixel + 8) >> 4) + numExtraMuxBits
    flatness_min_qp = flatness_min_qp_table[table_sel]
    flatness_max_qp = flatness_max_qp_table[table_sel]
    rc_quant_incr_limit0 = rc_quant_incr_limit_table[table_sel]
    rc_quant_incr_limit1 = rc_quant_incr_limit_table[table_sel]
    rc_buf_thresh = np.array([896, 1792, 2688, 3584, 4480, 5376, 6272, 6720, 7168, 7616, 7744, 7872, 8000, 8064])
    rc_range_parameters = rc_range_parameters_table[table_sel]
    second_line_bpg_offset = native_420 * 12
    nsl_bpg_offset = int(ceil(float(second_line_bpg_offset << 11)) / (slice_height - 1))
    second_line_offset_adj = 512 * native_420

    if (native_422 or native_420):
        grpcnt = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))
        maxOffset = compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, bits_per_pixel,
                                      first_line_bpg_offset, second_line_bpg_offset, slice_bpg_offset, nfl_bpg_offset,
                                      nsl_bpg_offset, grpcnt, native_420)
        maxOffset = max(maxOffset,
                        compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, bits_per_pixel,
                                          first_line_bpg_offset, second_line_bpg_offset, slice_bpg_offset,
                                          nfl_bpg_offset,
                                          nsl_bpg_offset, groupsPerLine, native_420))
        maxOffset = max(maxOffset,
                        compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, bits_per_pixel,
                                          first_line_bpg_offset, second_line_bpg_offset, slice_bpg_offset,
                                          nfl_bpg_offset,
                                          nsl_bpg_offset, groupsPerLine * 2, native_420))
        rsbMin = rc_model_size - initial_fullness_offset + maxOffset

    else:
        rbsMin = int(rc_model_size - initial_fullness_offset + int(ceil(initial_xmit_delay * (bits_per_pixel >> 4)))) + groupsPerLine * first_line_bpg_offset

    hrdDelay = int((ceil(float(rbsMin) / (bits_per_pixel >> 4))))
    rcb_bits = int((ceil(float(hrdDelay) * (bits_per_pixel >> 4))))
    initial_dec_delay = hrdDelay - initial_xmit_delay
    initial_scale_value = int((8 * rc_model_size) / (rc_model_size - initial_offset))
    finalScaleValue = int(8 * rc_model_size / (rc_model_size - final_offset))
    slice_bpg_offset = int(ceil(float((1 << 11)) * (rc_model_size - initial_offset + numExtraMuxBits)/ (groupsTotal)))

    if finalScaleValue > 9:
        #scale_increment_interval = int(float(1 << 11) * final_offset / (float(finalScaleValue - 9) * nfl_bpg_offset + slice_bpg_offset + nfl_bpg_offset))
        scale_increment_interval = (int)(float((1 << 11)) * final_offset / (float((finalScaleValue - 9)) * (nfl_bpg_offset + slice_bpg_offset + nsl_bpg_offset)))

    else:
        scale_increment_interval = 0

    scale_decrement_interval = int(groupsPerLine / (initial_scale_value - 8))
    #nsl_bpg_offset = first_line_bpg_offset / (slice_height - 1)

    #### Insert ALL PPS to dict
    PPS["dsc_version_major"] = dsc_version_major
    PPS["dsc_version_minor"] = dsc_version_minor
    PPS["pps_identifier"] = pps_identifier
    PPS["bits_per_component"] = bits_per_component
    PPS["linebuf_depth"] = line_buf_depth
    PPS["blockpred_enable"] = block_pred_enable
    PPS["convert_rgb"] = convert_rgb
    PPS["simple_422"] = simple_422
    PPS["vbr_enable"] = vbr_enable
    PPS["bits_per_pixel"] = bits_per_pixel
    PPS["pic_height"] = pic_height
    PPS["pic_width"] = pic_width
    PPS["slice_height"] = slice_height
    PPS["slice_width"] = slice_width
    PPS["chunk_size"] = chunk_size
    PPS["initial_xmit_delay"] = initial_xmit_delay
    PPS["initial_dec_delay"] = initial_dec_delay
    PPS["initial_scale_value"] = initial_scale_value
    PPS["scale_increment_interval"] = scale_increment_interval
    PPS["scale_decrement_interval"] = scale_decrement_interval
    PPS["first_line_bpg_ofs"] = first_line_bpg_offset
    PPS["nfl_bpg_offset"] = nfl_bpg_offset
    PPS["slice_bpg_offset"] = slice_bpg_offset
    PPS["initlal_offset"] = initial_offset
    PPS["final_offset"] = final_offset
    PPS["flatness_min_qp"] = flatness_min_qp
    PPS["flatness_max_qp"] = flatness_max_qp
    # Rc parameter sets starts
    PPS["rc_model_size"] = rc_model_size
    PPS["rc_edge_factor"] = rc_edge_factor
    PPS["rc_quant_icnr_limit0"] = rc_quant_incr_limit0
    PPS["rc_quant_icnr_limit1"] = rc_quant_incr_limit1
    PPS["rc_tgt_offset_hi"] = rc_tgt_offset_hi
    PPS["rc_tgt_offset_lo"] = rc_tgt_offset_lo
    PPS["rc_buf_thresh"] = rc_buf_thresh
    PPS["rc_range_parameters"] = rc_range_parameters
    # Rc parameter sets end
    PPS["native_420"] = native_420
    PPS["native_422"] = native_422
    PPS["second_line_bpg_offset"] = second_line_bpg_offset
    PPS["nsl_bpg_offset"] = nsl_bpg_offset
    PPS["second_line_ofs_adj"] = second_line_offset_adj

    if PRINT_DEBUG_OPT:
        for key, val in PPS.items():
            print("%s : "%key, val)

    # Below parameters are constants calculated from PPS values
    PPS_CFG["numSsp"] = numSsps
    PPS_CFG["bitsPerPixel"] = bits_per_pixel >> 4 # Removed Fractional 4-bits, Original Value!
    PPS_CFG["maxSeSize_Y"] = maxSeSize_Y
    PPS_CFG["maxSeSize_C"] = maxSeSize_C
    PPS_CFG["numExtraMuxBits"] = numExtraMuxBits
    PPS_CFG["pixelsPerGroup"] = pixelsPerGroup
    PPS_CFG["groupsPerLine"] = groupsPerLine
    PPS_CFG["groupsTotal"] = groupsTotal
    PPS_CFG["rcb_bits"] = rcb_bits
    PPS_CFG["hrdDelay"] = hrdDelay

    ### heetak modified
    PPS_CFG['somewhat_flat_qp_thresh'] = 7 + (2 * (PPS['bits_per_component'] - 8))
    PPS_CFG['somewhat_flat_qp_delta'] = 4

    return [PPS, PPS_CFG]

def compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, bits_per_pixel, first_line_bpg_offset, second_line_bpg_offset, slice_bpg_offset, nfl_bpg_offset, nsl_bpg_offset, groupcount, native_420):
    #grpcnt = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))
    grpcnt = groupcount
    grpcnt_id = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))

    if grpcnt <= grpcnt_id:
        offset = int(ceil(grpcnt * pixelsPerGroup * bits_per_pixel))
    else:
        offset = int(ceil(grpcnt_id * pixelsPerGroup * bits_per_pixel)) - (((grpcnt - grpcnt_id) * slice_bpg_offset) >> 11)

    if grpcnt <= groupsPerLine:
        offset += grpcnt * first_line_bpg_offset
    else:
        offset += groupsPerLine * first_line_bpg_offset - (((grpcnt - groupsPerLine) * nfl_bpg_offset) >> 11)

    if native_420:
        if grpcnt <= grpcnt_id:
            offset -= (grpcnt * nsl_bpg_offset) >> 11
        elif grpcnt <= 2 * groupsPerLine:
            offset += (grpcnt - groupsPerLine) * second_line_bpg_offset - ((groupsPerLine * nsl_bpg_offset) >> 11)
        else:
            offset += (grpcnt - groupsPerLine) * second_line_bpg_offset - (((grpcnt - groupsPerLine) * nsl_bpg_offset) >> 11)

    return offset