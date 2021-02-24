import torch
import torch.optim as optim
import torch.nn.functional as F
import numpy as np

from config import *
from Net import Net


class PolicyValue:

    def __init__(self, model_file=None):
        self.board_width = board_width
        self.board_height = board_height
        # L2正则化
        self.l2_const = 1e-4
        # 策略价值网络
        self.net = Net().cuda()
        # 优化器
        self.optimizer = optim.Adam(self.net.parameters(), weight_decay=self.l2_const)

        # 如果有初始模型，则加载初始模型
        if model_file:
            net_params = torch.load(model_file)
            self.net.load_state_dict(net_params)
            print("model load success")

    def policy_value(self, state_batch):
        # 当前棋盘状态
        state_batch = torch.FloatTensor(state_batch).cuda()
        # 策略与价值
        log_act_probs, value = self.net(state_batch)
        act_probs = np.exp(log_act_probs.data.cpu().numpy())

        return act_probs, value.data.cpu().numpy()

    def policy_value_fn(self, board):
        # 可行的落子
        legal_positions = board.availables
        # 当前局面
        current_state = np.ascontiguousarray(board.current_state().reshape(-1, 4, self.board_width, self.board_height))
        # 棋盘上每个位置的落子概率及整个局面的评分
        log_act_probs, value = self.net(torch.from_numpy(current_state).cuda().float())
        act_probs = np.exp(log_act_probs.data.cpu().numpy().flatten())
        act_probs = zip(legal_positions, act_probs[legal_positions])
        value = value.data[0][0]
        return act_probs, value

    # 参数更新
    def train(self, state_batch, mcts_probs, winner_batch, lr):
        # 自我对弈数据
        state_batch = torch.FloatTensor(state_batch).cuda()
        # 落子策略
        mcts_probs = torch.FloatTensor(mcts_probs).cuda()
        # 胜者
        winner_batch = torch.FloatTensor(winner_batch).cuda()

        # 梯度清零
        self.optimizer.zero_grad()

        # 设置学习率
        for param_group in self.optimizer.param_groups:
            param_group['lr'] = lr

        # 策略和价值
        log_act_probs, value = self.net(state_batch)
        # loss = (z - v)^2 - pi^T * log(p) + c||theta||^2
        value_loss = F.mse_loss(value.view(-1), winner_batch)
        policy_loss = -torch.mean(torch.sum(mcts_probs * log_act_probs, 1))
        loss = value_loss + policy_loss
        # 反向传播
        loss.backward()
        # 更新参数
        self.optimizer.step()
        # 概率分布的熵
        entropy = -torch.mean(torch.sum(torch.exp(log_act_probs) * log_act_probs, 1))

        return loss.item(), entropy.item()

    # 保存模型
    def save_model(self, model_file):
        net_params = self.net.state_dict()
        torch.save(net_params, model_file)
