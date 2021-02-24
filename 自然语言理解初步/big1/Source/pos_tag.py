# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import json

import segment_dic


# 词性标注器
class pos:

    def __init__(self):
        init = './model/init_pos.json'
        transition = './model/transition_pos.json'
        emission = './model/emission_pos.json'
        self.Segment = segment_dic.segment()  # 分词器
        # 加载参数
        with codecs.open(init, 'r', encoding='utf-8') as file:
            self.init_seq = json.load(file)
        with codecs.open(transition, 'r', encoding='utf-8') as file:
            self.transition_seq = json.load(file)
        with codecs.open(emission, 'r', encoding='utf-8') as file:
            self.emission_seq = json.load(file)
        """
        26个基本词类标记
        （名词n、时间词t、处所词s、方位词f、数词m、量词q、区别词b、代词r、动词v、
        形容词a、状态词z、副词d、介词p、连词c、助词u、语气词y、叹词e、拟声词o、
        成语i、习惯用语l、简称j、前接成分h、后接成分k、语素g、非语素字x、标点符号w）
        """
        self.state_seq = ['Ag', 'a', 'ad', 'an', 'Bg', 'b', 'c', 'Dg',
                          'd', 'e', 'f', 'h', 'i', 'j', 'k', 'l',
                          'Mg', 'm', 'Ng', 'n', 'nr', 'ns', 'nt', 'nx',
                          'nz', 'o', 'p', 'q', 'Rg', 'r', 's', 'na',
                          'Tg', 't', 'u', 'Vg', 'v', 'vd', 'vn', 'vvn',
                          'w', 'Yg', 'y', 'z']

    # viterbi算法
    def viterbi(self, sentence):
        V = [{}]
        path = {}
        for state in self.state_seq:
            V[0][state] = self.init_seq[state] * self.emission_seq[state].get(sentence[0], 0)
            path[state] = [state]
        for i in range(1, len(sentence)):
            V.append({})
            new_path = {}
            for state in self.state_seq:
                state_path = (
                    [(V[i - 1][state_0] * self.transition_seq[state_0].get(state, 0) * self.emission_seq[state].get(
                        sentence[i], 0), state_0)
                     for state_0 in self.state_seq])
                (score, state_now) = max(state_path)
                V[i][state] = score
                new_path[state] = path[state_now] + [state]
            path = new_path
        (score, state_now) = max([(V[len(sentence) - 1][y], y) for y in self.state_seq])
        return path[state_now]  # 返回最大路径

    # 词性标注的外部接口
    def pos_tag(self, sent, need_cut=True):
        if need_cut:
            sent = self.Segment.bi_segment(sent)  # 双向最长匹配分词
        tag = self.viterbi(sent)
        for index in range(len(tag)):
            tag[index] = sent[index] + '/' + tag[index]
        return sent, tag  # 返回分词结果和词性
