import os
import sys

import cv2
import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.nn.functional as F
from PyQt5.QtWidgets import *
from torch import nn
from torchviz import make_dot

from classify import classify
from config import *
from makedata import Board


# 返回真实棋局状态和预测棋局状态
def get_data(path, i):
    # 判断文件路径
    if path[-2:] == "2/":
        img_path = path + "image_c/"
    else:
        img_path = path + "image/"
    # 读取图片
    image = cv2.imread(img_path + str(i) + ".png")
    image = image.astype(np.float)
    result = np.zeros(image.shape, dtype=np.float32)
    # 将图片标准化
    cv2.normalize(image, result, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_32F)
    result = torch.from_numpy(result)
    result = result.float()
    result = result.permute(2, 0, 1)
    result = torch.unsqueeze(result, dim=0)
    # 获取预测值
    table_hat = classifier(result)
    # 处理真实值
    table = np.load(path + "table/" + str(i) + ".npy")
    table = torch.from_numpy(table).long()
    table = table.view(-1)
    # 返回预测值和真实值
    return table_hat, table


# 训练
def train():
    # 判断文件路径
    path = ["./data/train2/", "./data/train2/"]
    loss_sum = 0
    for item in path:
        if item == "./data/train2/":
            files = os.listdir(item + "image_c")
        else:
            files = os.listdir(item + "image")
        # 加载文件
        for i in range(len(files)):
            # 获取真实值和预测值
            table_hat, table = get_data(item, i)
            # 梯度清零
            optim.zero_grad()
            # loss
            loss = F.nll_loss(table_hat, table)

            loss_sum += loss.item()
            # 反向传播
            loss.backward()
            # 更新参数
            optim.step()
    # 保存loss
    Loss_list.append(loss_sum)


# 测试
def test():
    path = ["./data/test2/", "./data/test1/"]
    correct = 0
    sum_pia = 0
    # 判断路径
    for item in path:
        if item == "./data/test2/":
            files = os.listdir(item + "image_c")
        else:
            files = os.listdir(item + "image")

        sum_pia += len(files) * size * size
        # 对每个图片进行测试
        for i in range(len(files)):
            table_hat, table = get_data(item, i)

            pred = torch.max(F.softmax(table_hat, dim=1), 1)[1]
            pred = pred.numpy()
            table = table.numpy()

            correct += sum(pred == table)
    # 获取正确率
    Correct_List.append(correct / sum_pia)
    print(correct / sum_pia)


# 画loss和correct
def draw():
    Epochs = np.array(range(1, epochs + 1))
    Loss = np.array(Loss_list)
    Correct = np.array(Correct_List)
    # 画散点图
    plt.scatter(Epochs, Loss, marker='.', s=10)
    plt.xlabel("epoch")
    plt.ylabel("loss")
    plt.show()
    # 画散点图
    plt.scatter(Epochs, Correct, marker='.', s=10)
    plt.xlabel("epoch")
    plt.ylabel("correct")
    plt.show()


# 展示一张图片
def show():
    path = "data/test1/"
    # 预测值与真实值
    table_hat, table = get_data(path, 0)

    pred = torch.max(F.softmax(table_hat, dim=1), 1)[1]
    pred = pred.view(15, 15).numpy()
    table = table.view(15, 15).numpy()
    # 通过Qt绘制图片
    app = QApplication(sys.argv)
    board = Board(table, pred)
    board.show()
    # 保存吐泡泡
    path = "report/"
    image = path + "image/image.png"
    board.pix.save(image)

    board.close()


if __name__ == "__main__":
    # 分类器
    classifier = classify()

    # 画网络结构图
    show_data = torch.randn(1, 3, 480, 480)
    net_plot = make_dot(classifier(show_data), params=dict(classifier.named_parameters()))
    net_plot.view()
    # 定义损失函数和优化器
    loss_function = nn.CrossEntropyLoss()
    optim = torch.optim.Adam(classifier.parameters(), lr=0.0001)
    # 训练
    for epoch in range(epochs):
        train()
        test()
    draw()
    # 展示一张图片
    show()
    # 保存模型
    model_path = "model/"
    torch.save(classifier, model_path + "classifier.pth")
    print("model has been saved")
