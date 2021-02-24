import torch
import torch.nn as nn
import torch.nn.functional as F

from config import *


# 策略价值网络
class Net(nn.Module):

    def __init__(self):
        super(Net, self).__init__()

        self.board_width = board_width
        self.board_height = board_height
        # 公共层
        self.conv1 = nn.Conv2d(4, 32, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        # 策略层
        self.act_conv1 = nn.Conv2d(128, 4, kernel_size=1)
        self.act_fc1 = nn.Linear(4 * board_width * board_height, board_width * board_height)
        # 价值层
        self.val_conv1 = nn.Conv2d(128, 2, kernel_size=1)
        self.val_fc1 = nn.Linear(2 * board_width * board_height, 64)
        self.val_fc2 = nn.Linear(64, 1)

    def forward(self, state):
        # 公共层
        x = F.relu(self.conv1(state))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))
        # 策略层
        x_pol = F.relu(self.act_conv1(x))
        x_pol = x_pol.view(-1, 4 * self.board_width * self.board_height)
        x_pol = F.log_softmax(self.act_fc1(x_pol), dim=1)
        # 价值层
        x_val = F.relu(self.val_conv1(x))
        x_val = x_val.view(-1, 2 * self.board_width * self.board_height)
        x_val = F.relu(self.val_fc1(x_val))
        x_val = torch.tanh(self.val_fc2(x_val))
        # 返回策略和价值
        return x_pol, x_val
