# DSC_CODEC Python Class
# THIS CLASS IS IMPLEMENTED TO CONVERT IMAGE FILE TO PPM EXTENSION IMAGE
# CONVERTED FILE WILL BE USED TO COMPRESS AND DECODE USING DSC C MODEL
# CODED BY IM SEUNGWOO

#          DRAFT : 2020.05.14
# FINAL REVISION : 2020.05.18

import os
from PIL import Image
import time
import shutil
from init_pps import *

class DSC_CODEC:

    def __init__(self):
        self.is_cfg_created = False
        self.is_image_converted = False
        self.is_list_created = False
        self.print_eps = True

    def dsc_set_cfg(self, cfg_dict = None, cfg_file_name = "test_py.cfg"):
        self.cfg_dict = cfg_dict
        self.cfg_file_name = cfg_file_name
        self.cfg_line = {}

        if cfg_dict["Bits_per_component"] not in [8, 10, 12, 14, 16]:
            raise ValueError("%d is unsupported 'Bits_per_component Value'" %cfg_dict["Bits_per_component"])

        with open("test.cfg", 'r') as cfg:
            for num, lines in enumerate(cfg):
                self.cfg_line[num] = lines

        self.cfg_line[6] = self.cfg_line[6][:10] + str(self.cfg_dict["DSC_MODE"]) + self.cfg_line[6][11:]
        self.cfg_line[31] = self.cfg_line[31][:14] + str(self.cfg_dict["slice_width"]) #+ self.cfg_line[31][15:]
        self.cfg_line[34] = self.cfg_line[34][:15] + str(self.cfg_dict["slice_height"])# + self.cfg_line[34][16:]
        self.cfg_line[38] = self.cfg_line[38][:21] + str(self.cfg_dict["BLOCK_PRED_ENABLE"]) + self.cfg_line[38][22:]
        self.cfg_line[43] = self.cfg_line[43][:15] + str(self.cfg_dict["USE_YUV_INPUT"]) + self.cfg_line[43][16:]
        self.cfg_line[44] = self.cfg_line[44][:15] + str(self.cfg_dict["SIMPLE_422"]) + self.cfg_line[44][16:]
        self.cfg_line[45] = self.cfg_line[45][:15] + str(self.cfg_dict["NATIVE_422"]) + self.cfg_line[45][16:]
        self.cfg_line[46] = self.cfg_line[46][:15] + str(self.cfg_dict["NATIVE_420"]) + self.cfg_line[46][16:]
        self.cfg_line[48] = self.cfg_line[48][:23] + str(self.cfg_dict["FULL_ICH_ERR_PRECISION"]) + self.cfg_line[48][24:]
        self.Bits_per_component = self.cfg_dict["Bits_per_component"]
        self.Bits_per_pixel = self.cfg_dict["Bits_per_pixel"]

        if self.cfg_dict["NATIVE_422"]:
            self.cfg_line[61] = self.cfg_line[61][:11] + "rc_%dbpc_%dbpp_422.cfg\n" % (self.Bits_per_component, self.Bits_per_pixel)
            self.cfg_line[84] = self.cfg_line[84][:16] + " 1" + self.cfg_line[84][17:]
        elif self.cfg_dict["NATIVE_420"]:
            self.cfg_line[61] = self.cfg_line[61][:11] + "rc_%dbpc_%dbpp_420.cfg\n" % (self.Bits_per_component, self.Bits_per_pixel)
            self.cfg_line[84] = self.cfg_line[84][:16] + " 0" + self.cfg_line[84][17:]
        else:
            self.cfg_line[61] = self.cfg_line[61][:11] + "rc_%dbpc_%dbpp.cfg\n" % (self.Bits_per_component, self.Bits_per_pixel)
            self.cfg_line[84] = self.cfg_line[84][:16] + " 0" + self.cfg_line[84][17:]

        try:
            with open(self.cfg_file_name, "wt") as cfg_py:
                for txt in self.cfg_line.values():
                    cfg_py.write(txt)
            print("Setting " + '"' + self.cfg_file_name + '"' + " file...")
            self.is_cfg_created = True

            return self

        except:
            raise NotImplementedError

    def convert_input_images(self, image_path = 'input_files/', output_extension = "ppm"):
        if self.cfg_dict["DSC_MODE"] == 2:
            self.is_image_converted = True
            print("Skip Image Converting Process in Decode Mode")
            return self

        self.image_path = image_path
        self.output_extension = output_extension
        self.input_file_list = os.listdir(self.image_path)
        self.output_file_list = []
        print("Converting Input Images to " + '"' + self.output_extension.upper() + '"' + " Extension")

        try:
            for fname in self.input_file_list:
                self.output_extension = output_extension
                name_index = fname.find(".")
                start_t = time.time()
                file_fmt = fname[-4:]

                if (file_fmt == ".dpx") or (file_fmt == ".ppm") or (file_fmt == ".yuv"):
                    print("This %s file is no need to be converted, just use this file" %fname)
                    self.output_extension = fname[-4:]
                    self.print_eps = False
                    shutil.copy(self.image_path + fname, os.getcwd())
                    fname_ = fname[: name_index] + self.output_extension

                else:
                    fname_ = fname[: name_index] + self.output_extension
                    im = Image.open(image_path + fname)
                    im.save(os.getcwd() + "/" + fname_)

                end_t = time.time()
                if self.print_eps:
                    print("Elapsed Time for convert Current Image : %.3fs" %(end_t - start_t))

                #fname_ = fname[: name_index] + self.output_extension
                self.output_file_list.append(fname_)
                self.is_image_converted = True

            return self

        except:
            raise NotImplementedError

    def dsc_set_image_list(self):
        if self.cfg_dict["DSC_MODE"] == 2:
            self.is_list_created = True
            print("Skip Setting Image List Process in Decode Mode")
            return self

        try:
            with open("test_list.txt", "w") as f:
                for f_name in self.output_file_list:
                    f.write(f_name + "\n")
            print("Writing Input File List")
            self.is_list_created = True

            return self

        except:
            raise NotImplementedError

    def start_dsc(self):
        self.is_dsc_ready = self.is_cfg_created & self.is_image_converted & self.is_list_created

        if self.is_dsc_ready:
            print("Starting DSC Model")
            self.dsc_path = os.getcwd()
            os.system("cd {}".format(self.dsc_path))
            os.system("DSC -F " + self.cfg_file_name)

            return self

        else:
            raise NotImplementedError("DSC is not ready to start")