import numpy as np
import cv2
from os import listdir
from os import makedirs

makedirs("imgsResized")

kernel = np.ones((3,3), np.uint8)

for folder in listdir("data"):
    makedirs("imgsResized/" + folder + "Resized")
    for img in listdir("data/" + folder):
        image = cv2.imread("data/" + folder + "/" + img)
        img_dilation = cv2.dilate(image, kernel, iterations=1)
        result = cv2.resize(img_dilation,(10,10), interpolation = cv2.INTER_CUBIC)
        cv2.imwrite("./imgsResized/" + folder + "Resized/" + img, result)
        