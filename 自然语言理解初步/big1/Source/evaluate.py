# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import time

import segment_dic
import segment_hmm
import pos_tag


class eval:

    def __init__(self):
        # 加载分词器和词性标注模型
        self.Segment_dic = segment_dic.segment()
        self.Segment_hmm = segment_hmm.segment()
        self.Pos_tag = pos_tag.pos()

    def accuracy(self, path, mode):
        data = codecs.open(path, 'r', encoding='utf-8')  # 打开测试文件
        start_time = time.time()  # 开始时间
        A = 0
        B = 0
        TP = 0
        # 双向最长匹配的字典分词
        if mode == "bi-ward":
            for line in data.readlines():
                line = line.strip()
                sentence = line.replace(' ', '')
                line = line.split()
                word_list = self.Segment_dic.bi_segment(sentence)
                A += len(line)
                B += len(word_list)
                for word in line:
                    if word in word_list:
                        TP += 1
                        word_list.remove(word)
            P = TP / B
            R = TP / A
            F = 2 * P * R / (P + R)
            end_time = time.time()
            cost = end_time - start_time
            return P, R, F, cost
        # hmm分词
        elif mode == "hmm":
            for line in data.readlines():
                line = line.strip()
                sentence = line.replace(' ', '')
                line = line.split()
                word_list = self.Segment_hmm.hmm_segment(sentence)
                A += len(line)
                B += len(word_list)
                for word in line:
                    if word in word_list:
                        TP += 1
                        word_list.remove(word)
            P = TP / B
            R = TP / A
            F = 2 * P * R / (P + R)
            end_time = time.time()
            cost = end_time - start_time  # 花费的时间
            return P, R, F, cost
        # 词性标注
        elif mode == "pos-tag":
            for line in data.readlines():
                line = line.strip()
                line = line.split()
                if len(line) == 0:
                    continue
                word_list = []
                for word in line:
                    position = word.index('/')
                    word_list.append(word[:position])
                _, tag_list = self.Pos_tag.pos_tag(word_list, need_cut=False)
                A += len(line)
                B += len(word_list)
                for word in line:
                    if word in tag_list:
                        TP += 1
                        tag_list.remove(word)
            P = TP / B
            end_time = time.time()
            cost = end_time - start_time
            return P, cost

    def evaluate(self):
        # 分词测试文件路径
        test_cut = 'data/test_segment.txt'
        PP, RR, FF, COST = self.accuracy(test_cut, "bi-ward")
        print("分词：bi-ward")
        print("精确率：", round(PP, 3), "召回率：", round(RR, 3), "F1：", round(FF, 3), "花费时间：", round(COST, 3))
        PP, RR, FF, COST = self.accuracy(test_cut, "hmm")
        print("分词：hmm")
        print("精确率：", round(PP, 3), "召回率：", round(RR, 3), "F1：", round(FF, 3), "花费时间：", round(COST, 3))
        # 词性标注文件路径
        test_tag = 'data/test_tag.txt'
        PP, COST = self.accuracy(test_tag, "pos-tag")
        print("词性标注：hmm")
        print("准确率：", round(PP, 3), "花费时间：", round(COST, 3))
