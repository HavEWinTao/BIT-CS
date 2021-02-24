# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

import json
import codecs

from Tree import Tree  # 字典树


class load_data:

    def __init__(self):
        # 字、词、同形字
        self.zi_path = "data/zi.json"
        self.ci_path = "data/ci.json"
        self.homoform_path = "data/homoform.txt"

    # 读取所有的字
    def get_zi(self):
        path = self.zi_path
        zi = ""  # 所有字的字符串
        file = codecs.open(path, 'r', encoding='utf-8')
        data = json.load(file)
        for i in range(len(data)):
            zi += data[i]['zi']

        return zi

    # 读取所有的同形字
    def get_homoform(self):
        path = self.homoform_path
        homoform = {}
        data = codecs.open(path, 'r', encoding='utf-8')
        for line in data.readlines():
            zi = line.split()
            # 生成同形字互字典
            for i in range(len(zi)):
                temp = line.split()
                del temp[i]
                homoform[zi[i]] = temp

        return homoform

    # 读取所有的词
    def get_ci(self):
        path = self.ci_path
        ci = Tree()  # 保存词的字典树
        file = codecs.open(path, 'r', encoding='utf-8')
        data = json.load(file)
        for i in range(len(data)):
            # 将词和词频保存下来
            word = data[i]['ci']
            frequency = data[i]['frequency']
            ci[word] = int(frequency)

        return ci
