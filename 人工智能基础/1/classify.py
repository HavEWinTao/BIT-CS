import torch
import torch.nn as nn
import torch.nn.functional as F

from config import *


class classify(nn.Module):
    def __init__(self):
        super(classify, self).__init__()
        # 卷积层
        layer1 = nn.Sequential(
            nn.Conv2d(3, channel1, 4, 4),
            nn.BatchNorm2d(channel1),
            nn.ReLU(),
            nn.MaxPool2d(2, 2)
        )
        self.layer1 = layer1
        # 卷积层
        layer2 = nn.Sequential(
            nn.Conv2d(channel1, channel2, 2, 2),
            nn.ReLU(),
            nn.MaxPool2d(2, 2)
        )
        self.layer2 = layer2
        # 线性层
        layer3 = nn.Sequential(
            nn.Linear(channel2, dim1),
            nn.ReLU()
        )
        self.layer3 = layer3
        # 线性层
        layer4 = nn.Sequential(
            nn.Linear(dim1, 3),
        )
        self.layer4 = layer4

    # 前向传播
    def forward(self, x):
        conv1 = self.layer1(x)
        conv2 = self.layer2(conv1)
        conv2 = torch.squeeze(conv2, dim=0)
        conv2 = conv2.view(24, size * size)
        conv2 = conv2.permute(1, 0)
        linear1 = self.layer3(conv2)
        linear2 = self.layer4(linear1)
        return F.log_softmax(linear2, dim=1)  # 对结果取softmax
