import numpy as np

from PureMCTS import PureMCTS
from AlphaZeroMCTS import AlphaZeroMCTS


# 蒙特卡洛树搜索
class MCTSPlayer(object):

    def __init__(self, c_puct=5, playout_num=2000):
        self.mcts = PureMCTS(c_puct, playout_num)

    # 设定player的编号
    def set_player_id(self, p):
        self.player = p

    # 重置AIPlayer
    def reset_player(self):
        self.mcts.update_tree(-1)

    # 确定最终落子动作
    def get_action(self, board):
        # 可以到达的落子位置
        sensible_moves = board.availables
        if len(sensible_moves) > 0:
            move = self.mcts.get_move(board)
            # 重置根节点
            self.mcts.update_tree(-1)
            return move


# 执行蒙特卡洛搜索树的逻辑
class AIPlayer(object):

    def __init__(self, policy_value_function, c_puct=5, playout_num=2000, is_selfplay=0):
        self.mcts = AlphaZeroMCTS(policy_value_function, c_puct, playout_num)
        self._is_selfplay = is_selfplay

    # 设定player的编号
    def set_player_id(self, p):
        self.player = p

    # 重置AIPlayer
    def reset_player(self):
        self.mcts.update_tree(-1)

    # 确定最终落子动作
    # 可以在自我对弈和人机对战时使用
    def get_action(self, board, temp=1e-3, return_prob=0):
        # 可以到达的落子位置
        sensible_moves = board.availables
        # 落子概率
        move_probs = np.zeros(board.width * board.height)
        if len(sensible_moves) > 0:
            # 获取该棋盘状态下的所有的可行动作及其每个动作对应的概率
            acts, probs = self.mcts.get_probs(board, temp)
            move_probs[list(acts)] = probs
            # 是self-play
            if self._is_selfplay:
                move = np.random.choice(acts, p=0.75 * probs + 0.25 * np.random.dirichlet(0.3 * np.ones(len(probs))))
                # 更新根节点，复用搜索子树
                self.mcts.update_tree(move)
            else:
                move = np.random.choice(acts, p=probs)
                # 重置根节点
                self.mcts.update_tree(-1)

            if return_prob:
                return move, move_probs
            # 返回落子位置
            else:
                return move
