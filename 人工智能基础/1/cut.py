import os

import cv2

# 切割图片
path = ["./data/test2/", "./data/train2/"]
for item in path:
    files = os.listdir(item + "image/")
    print(len(files))
    for i in range(len(files)):
        img = cv2.imread(item + "image/" + str(i) + ".png")
        img = img[:480, :480]
        cv2.imwrite(item + "image_c/" + str(i) + ".png", img)
