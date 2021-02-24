from operator import itemgetter

import Const

# 制作生成one-hot编码的字典，取出现频率最高的单词
def MakeDir():
    txt = open(Const.train_out, encoding='utf-8').readlines()

    dictionary_nt = {}
    dictionary_other = {}

    for line in txt:
        pair = line.split()
        word = pair[0]
        if pair[1] == Const.target:
            if word not in dictionary_nt:
                dictionary_nt[word] = 1
            else:
                dictionary_nt[word] = dictionary_nt[word] + 1
        if pair[1] != Const.target:
            if word not in dictionary_other:
                dictionary_other[word] = 1
            else:
                dictionary_other[word] = dictionary_other[word] + 1

    dictionary_nt = sorted(dictionary_nt.items(), key=itemgetter(1), reverse=True)
    dictionary_other = sorted(dictionary_other.items(), key=itemgetter(1), reverse=True)

    token = {}

    for i in range(Const.Length_nt - 1):
        token[dictionary_nt[i][0]] = len(token)

    for i in range(Const.Length_other):
        token[dictionary_other[i][0]] = len(token)

    return token
