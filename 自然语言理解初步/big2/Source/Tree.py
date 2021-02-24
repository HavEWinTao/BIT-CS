# coding=utf-8
# Author:fan hongtao
# Date:2020-11-26

# 字典树的结点
class Node(object):
    # 初始化
    def __init__(self, value) -> None:
        self._children = {}
        self._value = value

    # 增加结点
    def _add_child(self, char, value, overwrite=False):
        child = self._children.get(char)
        if child is None:
            child = Node(value)
            self._children[char] = child
        elif overwrite:
            child._value = value
        return child


# 字典树
class Tree(Node):
    def __init__(self) -> None:
        super().__init__(None)

    # 判断是否存在
    def __contains__(self, item):
        return self[item] is not None

    # 返回下标
    def __getitem__(self, item):
        state = self
        for char in item:
            state = state._children.get(char)
            if state is None:
                return None
        return state._value

    # 插入值
    def __setitem__(self, key, value):
        state = self
        for i, char in enumerate(key):
            if i < len(key) - 1:
                state = state._add_child(char, None, False)
            else:
                state = state._add_child(char, value, True)
