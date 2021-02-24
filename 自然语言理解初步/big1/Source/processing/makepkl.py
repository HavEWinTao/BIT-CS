# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import re
import pandas as pd
import numpy as np
from collections.abc import Iterable
import pickle
from sklearn.model_selection import train_test_split


# 将多维列表转换为1维列表
def flatten(Object):
    result = []
    for el in Object:
        if isinstance(Object, Iterable) and not isinstance(el, str):
            result.extend(flatten(el))
        else:
            result.append(el)
    return result


# 数据文件
data = codecs.open('../data/data_ner.txt', encoding='utf-8')
output = codecs.open('./process_1.txt', 'w', encoding='utf-8')
for line in data.readlines():
    line = line.split('  ')
    i = 1
    while i < len(line) - 1:
        if line[i][0] == '[':
            output.write(line[i].split('/')[0][1:])
            i += 1
            while i < len(line) - 1 and line[i].find(']') == -1:
                if line[i] != '':
                    output.write(line[i].split('/')[0])
                i += 1
            output.write(line[i].split('/')[0].strip() + '/' + line[i].split('/')[1][-2:] + ' ')
        elif line[i].split('/')[1] == 'nr':
            word = line[i].split('/')[0]
            i += 1
            if i < len(line) - 1 and line[i].split('/')[1] == 'nr':
                output.write(word + line[i].split('/')[0] + '/nr ')
            else:
                output.write(word + '/nr ')
                continue
        else:
            output.write(line[i] + ' ')
        i += 1
    output.write('\n')
data.close()
output.close()
# 将所有的命名实体用B、M、E表示次序
data = codecs.open('./process_1.txt', 'r', encoding='utf-8')
output = codecs.open('./process_2.txt', 'w', encoding='utf-8')
for line in data.readlines():
    line = line.split(' ')
    i = 0
    while i < len(line) - 1:
        if line[i] == '':
            i += 1
            continue
        word = line[i].split('/')[0]
        tag = line[i].split('/')[1]
        if tag == 'nr' or tag == 'ns' or tag == 'nt':
            output.write(word[0] + "/B_" + tag + " ")
            for j in word[1:len(word) - 1]:
                if j != ' ':
                    output.write(j + "/M_" + tag + " ")
            output.write(word[-1] + "/E_" + tag + " ")
        else:
            for wor in word:
                output.write(wor + '/O ')
        i += 1
    output.write('\n')
data.close()
output.close()
# 将每一行分割开
data = codecs.open('./process_2.txt', 'r', encoding='utf-8')
output = codecs.open('./process_3.txt', 'w', encoding='utf-8')
texts = data.read()
sentences = re.split('[，。！？、‘’“”:]/[O]', texts)
for sentence in sentences:
    if sentence != " ":
        output.write(sentence.strip() + '\n')
data.close()
output.close()

max_len = 60  # 一个句子的最大长度


# 将字转换为字的索引值
def X_padding(words):
    ids = list(word_id[words])
    if len(ids) >= max_len:
        return ids[:max_len]
    ids.extend([0] * (max_len - len(ids)))
    return ids


# 将标签转换为标签的索引值
def y_padding(tags):
    ids = list(tag_id[tags])
    if len(ids) >= max_len:
        return ids[:max_len]
    ids.extend([0] * (max_len - len(ids)))
    return ids


entity = list()  # 每行的字
labels = list()  # 每行的标签
tags = set()  # 所有的标签
tags.add('')
data = codecs.open('process_3.txt', 'r', 'utf-8')
# 将出现命名实体的行保存
for line in data.readlines():
    line = line.split()
    line_data = []
    line_label = []
    num = 0  # 命名实体词的数目
    for word in line:
        word = word.split('/')
        line_data.append(word[0])
        line_label.append(word[1])
        tags.add(word[1])
        if word[1] != 'O':
            num += 1
    if num != 0:
        entity.append(line_data)
        labels.append(line_label)
data.close()
# 将文字变成索引值
all_words = flatten(entity)
sr_allwords = pd.Series(all_words)
sr_allwords = sr_allwords.value_counts()
set_words = sr_allwords.index
set_ids = range(1, len(set_words) + 1)
word_id = pd.Series(set_ids, index=set_words)
id_word = pd.Series(set_words, index=set_ids)
# 将标签变成索引值
tags = [i for i in tags]
tag_ids = range(len(tags))
tag_id = pd.Series(tag_ids, index=tags)
id_tag = pd.Series(tags, index=tag_ids)
# 未出现过的字用unknow表示
word_id["unknow"] = len(word_id) + 1
id_word[len(word_id)] = "unknow"
# 将数据用DataFrame格式储存
df_data = pd.DataFrame({'words': entity, 'tags': labels}, index=range(len(entity)))
df_data['x'] = df_data['words'].apply(X_padding)
df_data['y'] = df_data['tags'].apply(y_padding)
x = np.asarray(list(df_data['x'].values))
y = np.asarray(list(df_data['y'].values))
# 生成训练集和测试集
x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.2, random_state=43)
# 保存数据文件
output = codecs.open('../data/ner.pkl', 'wb')
pickle.dump(word_id, output)
pickle.dump(id_word, output)
pickle.dump(tag_id, output)
pickle.dump(id_tag, output)
pickle.dump(x_train, output)
pickle.dump(y_train, output)
pickle.dump(x_test, output)
pickle.dump(y_test, output)
