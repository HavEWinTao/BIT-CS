# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import json


# 分词器
class segment:
    def __init__(self):
        init = './model/init_cut.json'
        transition = './model/transition_cut.json'
        emission = './model/emission_cut.json'
        # 加载模型的参数
        with codecs.open(init, 'r', encoding='utf-8') as file:
            self.init_seq = json.load(file)
        with codecs.open(transition, 'r', encoding='utf-8') as file:
            self.transition_seq = json.load(file)
        with codecs.open(emission, 'r', encoding='utf-8') as file:
            self.emission_seq = json.load(file)
        # 状态序列
        self.state_sequence = ('B', 'M', 'E', 'S')

    # 维特比算法
    def viterbi(self, sentence):
        V = [{}]
        path = {}
        for state in self.state_sequence:
            V[0][state] = self.init_seq[state] * self.emission_seq[state].get(sentence[0], 0)
            path[state] = [state]
        for i in range(1, len(sentence)):
            V.append({})
            new_path = {}
            for state in self.state_sequence:
                state_path = (
                    [(V[i - 1][state_0] * self.transition_seq[state_0].get(state, 0) * self.emission_seq[state].get(
                        sentence[i], 0),
                      state_0)
                     for state_0 in self.state_sequence if V[i - 1][state_0] > 0])
                if not state_path:
                    (score, state_now) = (0.0, 'S')
                else:
                    (score, state_now) = max(state_path)
                V[i][state] = score
                new_path[state] = path[state_now] + [state]
            path = new_path
        (score, state_now) = max([(V[len(sentence) - 1][state], state) for state in self.state_sequence])
        return score, path[state_now]

    # 分词的外部接口
    def hmm_segment(self, sentence):
        score, tag_list = self.viterbi(sentence)
        word_list = list()
        word = list()
        for index in range(len(tag_list)):
            if tag_list[index] == 'S':
                word.append(sentence[index])
                word_list.extend(word)
                word = []
            elif tag_list[index] in ['B', 'M']:
                word.append(sentence[index])
            elif tag_list[index] == 'E':
                word.append(sentence[index])
                word_list.append(''.join(word))
                word = []
        return word_list
