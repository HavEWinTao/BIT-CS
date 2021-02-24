# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import json

data = './data/train_hmm.txt'  # 训练数据
train = codecs.open(data, encoding='utf-8')

# 参数保存位置
init = './model/init_cut.json'
transition = './model/transition_cut.json'
emission = './model/emission_cut.json'


# 保存训练好的参数到文件中
def save_model(seq, path):
    jsObject = json.dumps(seq)
    file = codecs.open(path, 'w', encoding='utf-8')
    file.write(jsObject)
    file.close()


line_num = 0  # 总行数

'''
S:单字词
B:词的开头
M:词的中间
E:词的末尾
'''
state_sequence = ['B', 'M', 'E', 'S']  # 状态序列

init_seq = {}  # 初始状态概率向量
transition_seq = {}  # 状态转移概率矩阵
emission_seq = {}  # 发射概率矩阵

count_seq = {}  # 每个状态出现的次数

# 初始化
for state in state_sequence:
    transition_seq[state] = {}
    for stat in state_sequence:
        transition_seq[state][stat] = 0.0
# 初始化
for state in state_sequence:
    init_seq[state] = 0.0
    emission_seq[state] = {}
    count_seq[state] = 0


# 得到一个句子的状态
def get_state(words):
    word_state = []
    if len(words) == 1:
        word_state.append('S')
    else:
        M_num = len(words) - 2
        M_list = ['M'] * M_num
        word_state.append('B')
        word_state.extend(M_list)
        word_state.append('E')

    return word_state


for line in train.readlines():
    line_num += 1
    line = line.strip()
    if not line:
        continue
    # 将每一行的单词拆分
    word_seq = line.split()
    char_seq = []
    line_state = []
    # 得到每一行中的字和字的状态
    for word in word_seq:
        line_state.extend(get_state(word))
        for char in word:
            char_seq.append(char)
    # 训练参数
    for i in range(len(line_state)):
        if i == 0:
            init_seq[line_state[i]] += 1
            count_seq[line_state[i]] += 1
        else:
            transition_seq[line_state[i - 1]][line_state[i]] += 1
            count_seq[line_state[i]] += 1
            if char_seq[i] not in emission_seq[line_state[i]]:
                emission_seq[line_state[i]][char_seq[i]] = 0.0
            else:
                emission_seq[line_state[i]][char_seq[i]] += 1
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
