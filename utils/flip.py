import numpy as np
import cv2
from os import listdir, makedirs, path

for folder in listdir("data"):
    if(folder == "3"):
        break
    for img in listdir("data/" + folder):
        image = cv2.imread("data/" + folder + "/" + img)
        image = cv2.flip(image, 1)
        cv2.imwrite("data/" + folder + "/" + img, image)