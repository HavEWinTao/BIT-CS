# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

import codecs
import time

from AutoCorrect import AtuoCorrect


# 测试器
class evaluator:
    def __init__(self):
        # 纠正器
        self.Corrector = AtuoCorrect()
        self.test = []
        self.tag = []

        self.real = []

        self.all = []

        # 加载测试数据
        test_file = codecs.open("data/test.txt", "r", encoding='utf-8')
        for line in test_file.readlines():
            line = line.split()
            self.tag.append(line[0])
            self.test.append(line[1])
        test_file.close()

        # 加载真实的数据
        real_file = codecs.open("data/real.txt", "r", encoding='utf-8')
        for line in real_file.readlines():
            line = line.strip()
            self.real.append(line)

        # 加载测试数据
        all_file = codecs.open("data/exclude.txt", "r", encoding='utf-8')
        for line in all_file.readlines():
            line = line.strip()
            self.all.append(line)
        all_file.close()

    # 测试函数接口，供主程序调用
    def eval(self):
        # 记录测试花费的时间
        start = time.time()

        num = 0  # 正确的句子的数目
        # 对测试集中所有的句子进行更正
        for i in range(len(self.test)):
            correct_sent = self.Corrector.correct(self.test[i])
            # 输出原始句子和更正后的句子
            print("输入的句子：" + self.test[i])
            print("纠错后的句子：" + correct_sent)
            print()
            # 判断更正后的句子是否正确
            if correct_sent == self.real[i]:
                num += 1

        end = time.time()
        # 正确率和花费时间
        return round((num / len(self.test)), 3), round(end - start, 3)

    def cross_eval(self):
        # 记录测试花费的时间
        start = time.time()

        num = 0  # 正确的句子的数目
        # 对测试集中所有的句子进行更正
        for i in range(1150, len(self.all)):
            correct_sent = self.Corrector.correct(self.all[i])
            # 句子是否正确
            if correct_sent == self.all[i]:
                num += 1

        end = time.time()
        # 正确率和花费时间
        return round((num / len(self.all)), 3), round(end - start, 3)
