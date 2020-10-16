import numpy as np
import cv2
from matplotlib import pyplot as plt
from PIL import Image

image = np.zeros([1080, 1920, 3], dtype = np.uint8);

## for RGBW image...
# image[:, 0 : 480, 0] = 255
# image[:, 480 : 960, 1] = 255
# image[:, 960 : 1440, 2] = 255
# image[:, 1440 :, :] = 255

## for Red only image...
image[:, :, 0] = 255
image[:, :, 1] = 0
image[:, :, 2] = 0

# cv2.imshow("image.jpg", image)
# plt.imshow(image)
# plt.show()
new_im = Image.fromarray(image)
new_im.show()
new_im.save("RED_image.bmp")
# new_im.save("RGBW_image.bmp")
