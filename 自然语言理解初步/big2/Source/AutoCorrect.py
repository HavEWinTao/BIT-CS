# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

from pypinyin import lazy_pinyin
import jieba
import string

from load_data import load_data


class AtuoCorrect:

    def __init__(self):
        # 加载数据
        Dataloader = load_data()
        self.zi = Dataloader.get_zi()
        self.homoform = Dataloader.get_homoform()
        self.ci = Dataloader.get_ci()
        # 标签符号
        self.punctuation = string.punctuation
        self.punctuation += "。，？：；｛｝［］‘“”《》／！％……（）、"

    # 对单词进行扩展
    def expand(self, ci):
        # 分割词
        splits = [(ci[:i], ci[i:]) for i in range(len(ci) + 1)]
        # 删除某个字
        deletes = [L + R[1:] for L, R in splits if R]
        # reserve
        transposes = [L + R[1] + R[0] + R[2:] for L, R in splits if len(R) > 1]
        # 替换字
        replaces = [L + c + R[1:] for L, R in splits if R for c in self.zi]
        # 插入字
        inserts = [L + c + R for L, R in splits for c in self.zi]

        return set(deletes + transposes + replaces + inserts)

    # 判断词是否存在
    def exist(self, possible_ci):

        exist_ci = set()
        # 遍历每一个单词进行判断
        for ci in possible_ci:
            if ci in self.ci:
                exist_ci.add(ci)

        return list(exist_ci)

    # 返回可能的词
    def get_possible_ci(self, error_ci):

        possible_1 = []
        possible_2 = []
        possible_3 = []
        # 获取单词的拼音
        pinyin = lazy_pinyin(error_ci)
        # 对可能的词进行拓展
        expand_ci = self.expand(error_ci)
        # 判断拓展的词是否存在
        possible_ci = self.exist(expand_ci)

        for ci in possible_ci:
            # 获取可能的词的拼音
            possible_pinyin = lazy_pinyin(ci)
            # 拼音相同的优先级最高
            if possible_pinyin == pinyin:
                possible_1.append(ci)
            # 第一个字的拼音相同的优先级其次
            elif possible_pinyin[0] == pinyin[0]:
                possible_2.append(ci)
            else:
                possible_3.append(ci)

        return possible_1, possible_2, possible_3

    def add_homoform(self, res, error_ci):
        # 对词中的字，如果存在同形字，则进行替换
        for i in range(len(error_ci)):
            if error_ci[i] in self.homoform:  # 如果这个字存在同形字
                temp = error_ci

                for item in self.homoform[error_ci[i]]:
                    temp = list(temp)
                    temp[i] = item
                    temp = ''.join(temp)
                    res.append(''.join(temp))

        res = set(res)
        res = list(res)

        return self.exist(res)

    # 返回一个列表中的单词中概率最大的那个
    def max_frequency(self, possible):

        ci = possible[0]
        frequency = self.ci[ci]

        for item in possible:
            if item in self.ci:
                if self.ci[item] > frequency:
                    ci = item
        return ci

    # 对单词进行修正
    def get_correct(self, error_ci):
        # 按概率从高到低的三个列表
        possible_1, possible_2, possible_3 = self.get_possible_ci(error_ci)
        # 对列表二中的单词进行同形字纠正
        possible_2 = self.add_homoform(possible_2, error_ci)

        if possible_1:
            return self.max_frequency(possible_1)
        elif possible_2:
            return self.max_frequency(possible_2)
        elif possible_3:
            return self.max_frequency(possible_3)
        else:
            return []

    # 同形字修正
    def homoform_correct(self, word_1, word_2):
        # 字1和字2可能形成的单词的组合
        words = []
        # 同形字替换
        if word_1 in self.homoform:
            for item in self.homoform[word_1]:
                temp = item + word_2
                words.append(temp)

        if word_2 in self.homoform:
            for item in self.homoform[word_2]:
                temp = word_1 + item
                words.append(temp)
        # 合并
        words = set(words)
        words = list(words)

        res = []
        # 判断同形字替换后的单词是否为可能单词
        for word in words:
            if word in self.ci:
                res.append(word)
        if len(res) != 0:  # 同形字替换是否成功？
            return self.max_frequency(res)  # 返回概率最大的
        else:
            return res

    # 句子纠错的接口，供外部函数调用
    def correct(self, error_sentence, show=True):
        # 对输入的句子使用jieba分词
        cut = jieba.cut(error_sentence, cut_all=False)
        # 将所有分词后的单词以list格式存储
        words = "\t".join(cut).split("\t")
        # 正确的句子
        correct_sentence = ""
        # 遍历每一个单词
        for i in range(len(words)):
            # 当前单词
            word = words[i]
            # 因为错别字可能导致jieba将一个单词分为两个字
            # 所以进行同形不同音纠正（发音可能相同，但主要面对上面说的情况
            if i != 0 and (len(word) == 1 and len(words[i - 1]) == 1):
                # 两个连续出现的单个字并且这两个字都不是符号
                if (word not in self.punctuation) and (words[i - 1] not in self.punctuation):
                    temp = self.homoform_correct(words[i - 1], words[i])
                    # 如果能将这两个字纠正为一个词
                    if temp:
                        # 删除已保存的句子的最后一个字
                        correct_sentence = correct_sentence[:-1]
                        word = temp  # 修改word值

            correct_word = word
            # 如果当前字不是符号
            if word not in self.punctuation:
                # 如果当前的词在词典中未出现过
                if word not in self.ci:
                    # 对当前词进行修正
                    correct_word = self.get_correct(word)
                    if correct_word:
                        # 是否展示修改的错误的词
                        if show:
                            print(word, " 更改为： ", correct_word)
                    else:
                        correct_word = word
            # 正确的句子
            correct_sentence += correct_word

        return correct_sentence
