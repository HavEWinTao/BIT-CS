# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs

# 数据文件
data = codecs.open('../data/train_dic.txt', encoding='utf-8')

dictionary = []
# 统计所有出现过的词
for line in data.readlines():
    words = line.split()
    for word in words:
        if word not in dictionary:
            dictionary.append(word)
data.close()
# 将所有的词按频率排序
dictionary.sort()
# 保存字典
dic = codecs.open('../dictionary/words.dict', 'w', 'utf-8')
for word in dictionary:
    dic.write(word + '\n')
