import random
import numpy as np
from collections import defaultdict, deque
from Game import Game
from Board import Board
from Player import MCTSPlayer, AIPlayer
from PolicyValue import PolicyValue
from config import *


class AlphaZero:

    def __init__(self, init_model=None):
        # 棋盘相关参数
        self.board_width = board_width
        self.board_height = board_height
        # 用于游戏的棋盘
        self.board = Board()
        self.game = Game(self.board)

        # 训练相关参数
        self.lr = 2e-3
        self.lr_coef = 1.0
        self.buffer_size = 10000
        self.batch_size = 512
        self.buffer = deque(maxlen=self.buffer_size)
        self.kl_targ = 0.02
        self.mcts_search_num = 2000

        # 保存模型的频率
        self.save_freq = 50
        # 训练的次数
        self.game_epoch = 1000
        # 每一轮更新网络参数的次数
        self.net_epoch = 5

        # 自对弈相关参数
        self.temp = 1.0
        self.playout_num = 400
        self.c_puct = 5

        # 最大胜率
        self.best_win_rate = 0.0
        # 该局的落子数
        self.move_num = 0

        # 训练的价值策略网络
        self.policy_value = PolicyValue(model_file=init_model)
        # 蒙特卡洛玩家
        self.mcts_player = AIPlayer(self.policy_value.policy_value_fn, self.c_puct, self.playout_num, is_selfplay=1)

    # 获取对局的等价数据
    def get_equal_data(self, play_data):
        # 扩展的等价数据
        extend_data = []

        for state, mcts_porb, winner in play_data:
            # 旋转和对称翻转
            for i in [1, 2, 3, 4]:
                # 旋转等价数据
                equi_state = np.array([np.rot90(s, i) for s in state])
                equi_mcts_prob = np.rot90(np.flipud(mcts_porb.reshape(self.board_height, self.board_width)), i)
                extend_data.append((equi_state, np.flipud(equi_mcts_prob).flatten(), winner))
                # 翻转等价数据
                equi_state = np.array([np.fliplr(s) for s in equi_state])
                equi_mcts_prob = np.fliplr(equi_mcts_prob)
                extend_data.append((equi_state, np.flipud(equi_mcts_prob).flatten(), winner))

        return extend_data

    # 收集一个完整自我对局的数据
    def collect_data(self):
        winner, play_data = self.game.start_self_play(self.mcts_player, self.temp)
        play_data = list(play_data)[:]
        # 执行的总步数
        self.move_num = len(play_data)

        play_data = self.get_equal_data(play_data)
        self.buffer.extend(play_data)

    # 更新策略价值函数的参数
    def update(self):
        # 从buffer中随机采样batch_size个数据
        mini_batch = random.sample(self.buffer, self.batch_size)
        # 将局面状态、落子概率、胜负分开
        state_batch = [data[0] for data in mini_batch]
        mcts_probs_batch = [data[1] for data in mini_batch]
        winner_batch = [data[2] for data in mini_batch]
        # 之前的
        last_pol, last_val = self.policy_value.policy_value(state_batch)
        for i in range(self.net_epoch):
            loss, entropy = self.policy_value.train(state_batch, mcts_probs_batch, winner_batch,
                                                    self.lr * self.lr_coef)
            pol, val = self.policy_value.policy_value(state_batch)
            kl = np.mean(np.sum(last_pol * (np.log(last_pol + 1e-10) - np.log(pol + 1e-10)), axis=1))
            if kl > self.kl_targ * 4:
                break
        # 自适应学习率
        if kl > self.kl_targ * 2 and self.lr_coef > 0.1:
            self.lr_coef /= 1.5
        elif kl < self.kl_targ / 2 and self.lr_coef < 10:
            self.lr_coef *= 1.5

        print("loss: ", round(loss, 4), "entropy: ", round(entropy, 4))

    # 评估模型
    def evaluate(self, n_games=10):
        # 定义两个棋手
        current_mcts_player = AIPlayer(self.policy_value.policy_value_fn, self.c_puct, self.playout_num)
        pure_mcts_player = MCTSPlayer(c_puct=5, playout_num=self.mcts_search_num)
        win_cnt = defaultdict(int)
        for i in range(n_games):
            winner = self.game.start_play(current_mcts_player, pure_mcts_player, start_player=i % 2)
            win_cnt[winner] += 1
        # 赢:1分,平局:0.5分
        win_rate = 1.0 * (win_cnt[1] + 0.5 * win_cnt[-1]) / n_games
        print("num_playouts:{} ".format(self.mcts_search_num), end="")
        print("win:{},lose:{},tie:{}".format(win_cnt[1], win_cnt[2], win_cnt[-1]))
        # 胜率
        return win_rate

    # 对外提供的接口
    def run(self):
        for i in range(self.game_epoch):
            self.collect_data()
            print("batch ", i + 1, "  move_num: ", self.move_num)
            if len(self.buffer) > self.batch_size:
                self.update()
            # 评判模型
            if (i + 1) % 10 == 0:
                self.policy_value.save_model('./temp_policy.model')
            if (i + 1) % self.save_freq == 0:
                print("self playing")
                # 评判模型效果
                win_rate = self.evaluate()
                self.policy_value.save_model('./current_policy.model')
                if win_rate > self.best_win_rate:
                    print("New best model is saved")
                    self.best_win_rate = win_rate
                    self.policy_value.save_model('./best_policy.model')
                    # 当胜率为1的时候加深搜索深度
                    if self.best_win_rate == 1.0 and self.mcts_search_num < 5000:
                        self.mcts_search_num += 1000
                        self.best_win_rate = 0.0
