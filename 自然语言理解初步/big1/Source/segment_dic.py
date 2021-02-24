# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs


# 字典树的结点
class Node(object):
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


# 数一个分词结果里单字的个数
def count_signal(word_list):
    return sum(1 for word in word_list if len(word) == 1)


# 分词器
class segment:

    def __init__(self):

        self.counter = 1

        data = codecs.open('dictionary/words.dict', encoding='utf-8')
        user = codecs.open('dictionary/usr.dict', encoding='utf-8')
        self.dic_tree = Tree()
        self.dic_usr_tree = Tree()
        # 读取词典和用户词典，构造字典树
        for word in data.readlines():
            self.dic_tree[word[:-1]] = self.counter
            self.counter += 1
        data.close()
        for word in user.readlines():
            self.dic_usr_tree[word[:-1]] = self.counter
            self.counter += 1
        user.close()

    # 正向最长匹配
    def forward(self, text):
        word_list = []
        i = 0
        while i < len(text):
            longest_word = text[i]
            for j in range(i + 1, len(text) + 1):
                word = text[i:j]
                if (self.dic_tree[word] is not None) or (self.dic_usr_tree[word] is not None):
                    if len(word) > len(longest_word):
                        longest_word = word
            word_list.append(longest_word)
            i += len(longest_word)
        return word_list

    # 反向最长匹配
    def backward(self, text):
        word_list = []
        i = len(text) - 1
        while i >= 0:
            longest_word = text[i]
            for j in range(0, i):
                word = text[j:i + 1]
                if (self.dic_tree[word] is not None) or (self.dic_usr_tree[word] is not None):
                    if len(word) > len(longest_word):
                        longest_word = word
            word_list.insert(0, longest_word)
            i -= len(longest_word)
        return word_list

    # 双向最长匹配,供外部调用
    def bi_segment(self, text):
        f = self.forward(text)
        b = self.backward(text)
        # 返回词数较少的句子
        if len(f) < len(b):
            return f
        elif len(f) > len(b):
            return b
        else:
            if count_signal(f) < count_signal(b):
                return f
            else:
                return b

    # 向字典中添加词语
    def add_word(self):
        add = input("请输入要添加到字典中的词:")
        if self.dic_tree[add] is None:
            if self.dic_usr_tree[add] is None:
                self.dic_usr_tree[add] = self.counter
                self.counter += 1
                user_dic = codecs.open('dictionary/usr.dict', 'a+', encoding='utf-8')
                user_dic.write(add + '\n')
                user_dic.close()
                print("单词 " + add + " 已经成功添加到字典中")
            else:
                print("单词 " + add + " 已存在于用户字典中")
