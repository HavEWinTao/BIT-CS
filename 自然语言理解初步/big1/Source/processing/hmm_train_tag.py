# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import json

data = './data/train_tag.txt'  # 训练数据
train = codecs.open(data, encoding='utf-8')

# 参数保存位置
init = './model/init_pos.json'
transition = './model/transition_pos.json'
emission = './model/emission_pos.json'


# 保存训练好的参数到文件中
def save_model(seq, path):
    jsObject = json.dumps(seq)
    file = codecs.open(path, 'w', encoding='utf-8')
    file.write(jsObject)
    file.close()


line_num = 0  # 总行数

"""
26个基本词类标记
（名词n、时间词t、处所词s、方位词f、数词m、量词q、区别词b、代词r、动词v、
形容词a、状态词z、副词d、介词p、连词c、助词u、语气词y、叹词e、拟声词o、
成语i、习惯用语l、简称j、前接成分h、后接成分k、语素g、非语素字x、标点符号w）
"""

state_seq = ['Ag', 'a', 'ad', 'an', 'Bg', 'b', 'c', 'Dg',
             'd', 'e', 'f', 'h', 'i', 'j', 'k', 'l',
             'Mg', 'm', 'Ng', 'n', 'nr', 'ns', 'nt', 'nx',
             'nz', 'o', 'p', 'q', 'Rg', 'r', 's', 'na',
             'Tg', 't', 'u', 'Vg', 'v', 'vd', 'vn', 'vvn',
             'w', 'Yg', 'y', 'z']

init_seq = {}  # 初始状态概率向量
transition_seq = {}  # 状态转移概率矩阵
emission_seq = {}  # 发射概率矩阵

count_seq = {}  # 每个状态出现的次数

# 初始化
for state in state_seq:
    transition_seq[state] = {}
    for stat in state_seq:
        transition_seq[state][stat] = 0.0
    init_seq[state] = 0.0
    emission_seq[state] = {}
    count_seq[state] = 0

for line in train.readlines():
    line_num += 1
    line = line.strip()
    if not line:
        continue
    word_seq = line.split()
    line_seq = []
    line_state = []
    # 得到每一行中的词和字的状态
    for word in word_seq:
        position = word.index('/')
        line_seq.append(word[:position])
        line_state.append(word[position + 1:])
    # 训练参数
    for i in range(len(line_state)):
        count_seq[line_state[i]] += 1.0
        if i == 0:
            init_seq[line_state[i]] += 1.0
        else:
            transition_seq[line_state[i - 1]][line_state[i]] += 1.0
            if line_seq[i] not in emission_seq[line_state[i]]:
                emission_seq[line_state[i]][line_seq[i]] = 1.0
            else:
                emission_seq[line_state[i]][line_seq[i]] += 1.0
# 将数值转变为概率值
for key in init_seq:
    init_seq[key] = init_seq[key] * 1.0 / line_num
for key in transition_seq:
    for index in transition_seq[key]:
        transition_seq[key][index] = transition_seq[key][index] / count_seq[key]
for key in emission_seq:
    for word in emission_seq[key]:
        emission_seq[key][word] = emission_seq[key][word] / count_seq[key]
# 保存模型
save_model(init_seq, init)
save_model(transition_seq, transition)
save_model(emission_seq, emission)
