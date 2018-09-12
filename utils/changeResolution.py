import numpy as np
import cv2
from os import listdir, makedirs, path

makedirs("dataset_num_resized")

kernel = np.ones((3,3), np.uint8)

for folder in listdir("../dataset_num_camera"):
    if not path.exists("dataset_num_resized"):
        makedirs("dataset_num_resized/" + folder + "dataset_num_resized")
    for img in listdir("../dataset_num_camera/" + folder):
        image = cv2.imread("../dataset_num_camera/" + folder + "/" + img)
        img_dilation = cv2.dilate(image, kernel, iterations=1)
        result = cv2.resize(img_dilation,(10,10), interpolation = cv2.INTER_CUBIC)
        cv2.imwrite("./dataset_num_resized/" + folder + "Resized/" + img, result)
        