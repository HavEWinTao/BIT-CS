# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

import codecs
import random

random.seed(1)  # 随机数种子

# 原始数据的位置
data = codecs.open("../data/data.txt", "r", encoding='utf-8')

lines = []
# 将每一行的数据的空格去掉
for line in data.readlines():
    words = line.split()
    sentence = ""
    for word in words:
        sentence += word
    lines.append(sentence)
data.close()

sentences = []
# 保存每个句子
for line in lines:
    temp = line.split('。')
    for item in temp:
        sentences.append(item)

# 测试集，test_file需要自己手工修改
real_file = codecs.open("../data/real.txt", "w", encoding='utf-8')
test_file = codecs.open("../data/test.txt", "w", encoding='utf-8')
all_file = codecs.open("../data/all.txt", "w", encoding='utf-8')

test = []
# 因为句子数目较多，随机保存一些句子作为测试的句子
for item in sentences:
    if len(item) >= 15:
        test.append(item)
        all_file.write(item + '\n')

        if 2 < random.randint(0, 9) < 4:
            if 6 < random.randint(0, 9) < 8:
                real_file.write(item + '\n')
                test_file.write(item + '\n')
all_file.close()
test_file.close()
real_file.close()

print(len(test))
