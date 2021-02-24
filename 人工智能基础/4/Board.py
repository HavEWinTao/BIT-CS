import numpy as np

from config import *


# 游戏的棋盘
# 存储游戏状态
class Board(object):

    def __init__(self):
        self.width = board_width
        self.height = board_height
        self.states = {}  # 保存落子历史,即当前棋盘的状态
        self.n_in_row = n_in_row  # 连子数
        self.players = [1, 2]  # 玩家编号
        self.availables = list(range(self.width * self.height))  # 可落子位置
        self.current_player = -1  # 当前玩家
        self.last_move = -1  # 对方上一步落子位置

    # 对棋盘重新进行初始化
    def init_board(self, start_player=0):
        self.current_player = self.players[start_player]
        self.availables = list(range(self.width * self.height))
        self.states = {}
        self.last_move = -1

    # 将位置编号转换为二维坐标
    def move_to_location(self, move):
        h = move // self.width
        w = move % self.width
        return [h, w]

    # 将二维坐标转换为位置编号
    def location_to_move(self, location):
        h = location[0]
        w = location[1]
        move = h * self.width + w
        return move

    # 给出策略网络模型的输入
    def current_state(self):
        square_state = np.zeros((4, self.width, self.height))
        if self.states:
            moves, players = np.array(list(zip(*self.states.items())))
            move_curr = moves[players == self.current_player]
            move_oppo = moves[players != self.current_player]
            # 有棋子的位置为1
            square_state[0][move_curr // self.width,
                            move_curr % self.height] = 1.0
            # 有对手棋子的位置为1
            square_state[1][move_oppo // self.width,
                            move_oppo % self.height] = 1.0
            # 对手上一步落子位置为1
            square_state[2][self.last_move // self.width,
                            self.last_move % self.height] = 1.0
        # 如果当前是先手玩家，整个平面都为1
        if len(self.states) % 2 == 0:
            square_state[3][:, :] = 1.0  # indicate the colour to play
        return square_state[:, ::-1, :]

    # 落子位置正确后则可以正确落子
    def do_move(self, move):
        # 更新棋盘状态
        self.states[move] = self.current_player
        self.availables.remove(move)
        # 更换棋手
        if self.current_player == self.players[1]:
            self.current_player = self.players[0]
        else:
            self.current_player = self.players[1]
        # 记录上一步
        self.last_move = move

    def has_a_winner(self):
        states = self.states

        moved = list(set(range(board_width * board_height)) - set(self.availables))
        # 落子回合不够肯定没有胜者
        if len(moved) < n_in_row * 2 - 1:
            return False, -1
        # 遍历每一步棋
        for m in moved:
            h = m // board_width
            w = m % board_width
            player = states[m]

            # 遍历四个方向
            if (w in range(board_width - n_in_row + 1) and
                    len(set(states.get(i, -1) for i in range(m, m + n_in_row))) == 1):
                return True, player

            if (h in range(board_height - n_in_row + 1) and
                    len(set(states.get(i, -1) for i in range(m, m + n_in_row * board_width, board_width))) == 1):
                return True, player

            if (w in range(board_width - n_in_row + 1) and h in range(board_height - n_in_row + 1) and
                    len(set(
                        states.get(i, -1) for i in range(m, m + n_in_row * (board_width + 1), board_width + 1))) == 1):
                return True, player

            if (w in range(n_in_row - 1, board_width) and h in range(board_height - n_in_row + 1) and
                    len(set(
                        states.get(i, -1) for i in range(m, m + n_in_row * (board_width - 1), board_width - 1))) == 1):
                return True, player

        return False, -1

    # 判断游戏是否结束
    def game_end(self):
        win, winner = self.has_a_winner()
        if win:  # 游戏结束,返回胜者
            return True, winner
        elif not len(self.availables):  # 游戏结束,平局
            return True, -1
        return False, -1  # 游戏未结束

    # 获取当前玩家的编号
    def get_current_player(self):
        return self.current_player
