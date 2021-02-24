# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs

# 处理原始数据集保存到新文件中
data = codecs.open('../data/pku.txt', encoding='gbk')
output = codecs.open('../data/train_dic.txt', 'w', encoding='utf-8')

# data = codecs.open('data/train_tag.txt', encoding='utf-8')
# output = codecs.open('data/train_tag.txt', 'w', encoding='utf-8')

for line in data.readlines():
    words = line.split()
    flag = 0
    temp_word = ""
    # 提取[中央/n  人民/n  广播/vn  电台/n]nt  样式的词，最长词，如 :中央人民广播电台/n
    for word in words:
        tag = ""
        if word.startswith("["):
            flag = 1
            word = word[1:]
        elif "]" in word:
            flag = 2
            tag = word[word.index("]") + 1:]
            word = word[0:word.index("]")]
        temp = word.split("/")  # 分割命名实体
        if flag == 1:
            temp_word = temp_word + temp[0]
        elif flag == 2:
            temp_word = temp_word + temp[0]
            flag = 0
            # output.write(temp_word + "/" + tag + " ")
            output.write(temp_word + " ")
            temp_word = ""
        else:
            # output.write(temp[0] + "/" + temp[1] + " ")
            output.write(temp[0] + " ")
    output.write('\n')
output.close()
data.close()
