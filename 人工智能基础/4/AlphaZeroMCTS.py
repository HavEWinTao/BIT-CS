import numpy as np
import copy  # 深拷贝

from TreeNode import TreeNode


# 手动实现softmax函数
def softmax(x):
    probs = np.exp(x - np.max(x))
    probs /= np.sum(probs)
    return probs


# 蒙特卡洛树搜索
class AlphaZeroMCTS(object):

    def __init__(self, policy_value_fn, c_puct=5, n_playout=10000):
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
            action, node = node.select(self._c_puct)
            state.do_move(action)
        # 返回该叶子结点下的可行动作和对应的概率
        action_probs, leaf_value = self._policy(state)
        # 判断游戏是否结束
        end, winner = state.game_end()
        if not end:
            # 如果没有结束，拓展该节点
            node.expand(action_probs)
        else:
            if winner == -1:  # 平局
                leaf_value = 0.0
            else:  # 根据游戏胜负结果得到该节点对应的真实局面的评分
                leaf_value = (1.0 if winner == state.get_current_player() else -1.0)
        # 递归的更新整个搜索路径上的结点信息
        node.update_recursive(-leaf_value)

    # 获取该棋盘状态下的所有的可行动作及其每个动作对应的概率
    def get_probs(self, state, temp=1e-3):
        # 循环执行_playout
        for n in range(self._n_playout):
            state_copy = copy.deepcopy(state)
            self._playout(state_copy)
        # 每个子节点的访问次数
        act_visits = [(act, node._n_visits) for act, node in self._root._children.items()]
        acts, visits = zip(*act_visits)
        # 对应动作的概率
        act_probs = softmax(1.0 / temp * np.log(np.array(visits) + 1e-10))

        return acts, act_probs

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
