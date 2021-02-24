import numpy as np
import copy
from operator import itemgetter

from TreeNode import TreeNode


# 策略价值函数
def policy_value_fn(board):
    action_probs = np.ones(len(board.availables)) / len(board.availables)
    return zip(board.availables, action_probs), 0


# 返回落子概率
def rollout_policy_fn(board):
    action_probs = np.random.rand(len(board.availables))
    return zip(board.availables, action_probs)


# 蒙特卡洛树搜索
class PureMCTS(object):

    def __init__(self, c_puct=5, n_playout=10000):
        self._root = TreeNode(None, 1.0)
        self._policy = policy_value_fn  # 策略价值函数
        self._c_puct = c_puct  # 搜索程度
        self._n_playout = n_playout  # 循环执行的次数

    # 完整执行选择、拓展、评估和回传过程
    def _playout(self, state):
        # 从根节点出发
        node = self._root
        while 1:
            # 叶子结点则停止
            if node.is_leaf():
                break
            # 进行选择
            action, node = node.select(self._c_puct)
            state.do_move(action)
        # 返回该叶子结点下的可行动作和对应的概率
        action_probs, _ = self._policy(state)
        # 判断游戏是否结束
        end, winner = state.game_end()
        if not end:
            # 如果没有结束，拓展该节点
            node.expand(action_probs)
        # 根据游戏胜负结果得到该节点对应的真实局面的评分
        leaf_value = self._evaluate_rollout(state)
        # 递归的更新整个搜索路径上的结点信息
        node.update_recursive(-leaf_value)

    # 根据游戏胜负结果得到该节点对应的真实局面的评分
    def _evaluate_rollout(self, state, limit=1000):
        player = state.get_current_player()
        for i in range(limit):
            end, winner = state.game_end()
            # 游戏结束
            if end:
                break
            # 返回落子概率
            action_probs = rollout_policy_fn(state)
            # 最大概率的位置
            max_action = max(action_probs, key=itemgetter(1))[0]
            # 移动
            state.do_move(max_action)
        # 返回胜者
        if winner == -1:  # 平局
            return 0
        else:
            return 1 if winner == player else -1

    # 循环落子
    def get_move(self, state):
        for n in range(self._n_playout):
            state_copy = copy.deepcopy(state)
            self._playout(state_copy)
        return max(self._root._children.items(), key=lambda act_node: act_node[1]._n_visits)[0]

    # 在自我对弈过程中复用搜索的子树
    def update_tree(self, last_move):
        # 传入上一步最终执行的动作last_move
        # 如果该节点是当前搜索树根节点的某个子节点
        if last_move in self._root._children:
            # 以该结点为新的根节点
            self._root = self._root._children[last_move]
            self._root._parent = None
        else:
            self._root = TreeNode(None, 1.0)
