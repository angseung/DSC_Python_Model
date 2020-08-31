packet = LAYER_PARAMETER.RpRow_size + \
                 LAYER_PARAMETER.RpCol_size * 2**10 + \
                 LAYER_PARAMETER.Filter_size * 2**20 + \
                 BIT_INFO.BiasFraction * 2**30 + \
                 BIT_INFO.WeightFraction * 2**34 + \
                 BIT_INFO.InputFraction * 2**38 + \
                 BIT_INFO.OutputFraction * 2**42 + \
                 LAYER_PARAMETER.conv_layer_mode * 2**46 + \
                 LAYER_PARAMETER.layer_mode * 2**48 + \
                 LAYER_PARAMETER.ifmap_size * 2**52 + \
                 LAYER_PARAMETER.ofmap_size * 2**62 + \
                 LAYER_PARAMETER.ifmap_row_block_size * 2**78 + \
                 LAYER_PARAMETER.psum_row_block_size * 2**88 + \
                 LAYER_PARAMETER.ofmap_row_block_size * 2**98 + \
                 LAYER_PARAMETER.control * 2**108 + \
                 LAYER_PARAMETER.bias_cnt * 2**128 + \
                 LAYER_PARAMETER.weight_cnt * 2**152 +\
                 LAYER_PARAMETER.ifmap_cnt * 2**176 +\
                 LAYER_PARAMETER.ofmap_cnt * 2**200 + \
                 MAX_LAYER * 2**224 + \
                 TOP_IFMAP_SIZE * 2**256 + \
                 TOP_OCH_SIZE * 2**277 + \
                 LAYER_PARAMETER.filter_offset * 2**296 +\
                 LAYER_PARAMETER.bias_offset * 2**336 + \
                 LAYER_PARAMETER.ifmap_offset * 2**376 + \
                 LAYER_PARAMETER.ofmap_offset * 2**413 + \
                 LAYER_PARAMETER.och_cnt * 2**450 + \
                 LAYER_PARAMETER.ifmap_sram_offset * 2**458 + \
                 LAYER_PARAMETER.filter_sram_offset * 2**476 + \
                 LAYER_PARAMETER.bias_sram_offset * 2** 494

packet_byteline = packet.to_bytes(64,'big')

"""

    Packet_Info             DW[Bits]CDB     Bytes
    RpRow_size               10       10           1.25
    RpCol_size               10       20           2.5
    Filter_size               10       30           3.75
    Bias_fraction           4       34           4.25
    Weight_franction        4       38           4.75
    Ifmap_fraction           4       42           5.25
    ofmap_fraction           4       46           5.75
    conv_layer_mode           2       48           6
    layer_mode               4       52           6.5
    ifmap_size               10       62           7.75
    ofmap_size               10       72           9           <--- 1st Block
    zeros_padding           6       78           9.75
    ifmap_row_block_size    10       88           11
    Psum_row_block_size       10       98           12.25
    Ofmap_row_block_size   10       108           13.5
    control                   16       124           15.5
    0                       4       128           16
    bias_cnt               24       152           19
    weight_cnt               24       176           22          <--- 2nd Block
    ifmap_cnt               24       200           25
    ofmap_cnt               24       224           28
    MAX_layer               32       256           32
    fully_ifmap_size       21       277           34.625
    fully_och               16       293           36.625
    0                       3       296           37          <--- 3rd Block
    filter_offset           37       333           41.625
    0                       3       336           42
    bias_offset               37       373           46.625
    0                       3       376           47
    ifmap_offset           37       413           51.625
    ofmap_offset           37       450           56.25
    och_cnt                    8       458           57.25
    ifmap_sram_offset       18       476           59.5
    filter_sram_offset       18       494           61.75
    bias_sram_offset       18       512           64          <--- 4th Block

"""