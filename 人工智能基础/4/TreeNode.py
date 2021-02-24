import numpy as np


# 蒙特卡洛树的节点
class TreeNode(object):

    def __init__(self, parent, prior_p):
        self._parent = parent  # 当前创建节点的父节点
        self._children = {}  # 子节点
        self._n_visits = 0  # 当前节点的访问次数
        self._Q = 0  # 当前节点的平均行动价值
        self._u = 0  # 置信上限
        self._P = prior_p  # 当前节点被选择的先验概率

    # 拓展该叶子结点
    def expand(self, action_priors):
        # action_priors为一节点下可行动作和其对应的先验概率
        for action, prob in action_priors:
            # 向子节点中添加数据
            if action not in self._children:
                self._children[action] = TreeNode(self, prob)

    # 蒙特卡洛树搜索的选择步骤
    def select(self, c_puct):
        return max(self._children.items(),
                   key=lambda act_node: act_node[1].get_value(c_puct))

    # 节点访问次数加1
    # 增量更新Q
    def update(self, leaf_value):
        self._n_visits += 1
        self._Q += 1.0 * (leaf_value - self._Q) / self._n_visits

    # 递归从叶子结点更新到根节点
    # 调用update更新n和Q
    def update_recursive(self, leaf_value):
        if self._parent:
            self._parent.update_recursive(-leaf_value)
        self.update(leaf_value)

    # 计算Q+U
    def get_value(self, c_puct):
        self._u = (c_puct * self._P *
                   np.sqrt(self._parent._n_visits) / (1 + self._n_visits))
        return self._Q + self._u

    # 判断是否为叶子结点
    def is_leaf(self):
        return self._children == {}

    # 判断是否为根节点
    def is_root(self):
        return self._parent is None
