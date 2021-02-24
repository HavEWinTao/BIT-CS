# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import codecs
import pickle
import torch

# 加载数据
data = codecs.open('./data/ner.pkl', 'rb')
word_id = pickle.load(data)
id_word = pickle.load(data)
tag_id = pickle.load(data)
id_tag = pickle.load(data)
data.close()
# 开始、结束的标识
START_TAG = "<START>"
STOP_TAG = "<STOP>"
tag_id[START_TAG] = len(tag_id)
tag_id[STOP_TAG] = len(tag_id)
# 一个句子的最大长度
max_len = 60


# 将字变为索引
def padding(words):
    ids = list(word_id[words])
    if len(ids) >= max_len:
        return ids[:max_len]
    ids.extend([0] * (max_len - len(ids)))
    return ids


# 将将B、M、E命名实体的索引值转化为字
def calculate(x, y):
    res = []
    entity = []
    for j in range(len(x)):
        if x[j] == 0 or y[j] == 0:
            continue
        if id_tag[y[j]][0] == 'B':
            entity = [id_word[x[j].item()] + '/' + id_tag[y[j]]]
        elif id_tag[y[j]][0] == 'M' and len(entity) != 0 and entity[-1].split('/')[1][1:] == id_tag[y[j]][1:]:
            entity.append(id_word[x[j].item()] + '/' + id_tag[y[j]])
        elif id_tag[y[j]][0] == 'E' and len(entity) != 0 and entity[-1].split('/')[1][1:] == id_tag[y[j]][1:]:
            entity.append(id_word[x[j].item()] + '/' + id_tag[y[j]])
            res.append(entity)
            entity = []
        else:
            entity = []
    return res


# 命名实体识别
class recognition_entity:
    def __init__(self):
        self.model = torch.load('model/BiLSTM_CRF.pth')

    # 供外部调用的接口
    def ner(self, sentence):
        sentence = list(sentence)
        sentence = padding(sentence)
        sentence = torch.tensor(sentence, dtype=torch.long).cuda()
        _, predict = self.model(sentence)
        entity_hat = calculate(sentence, predict)
        return entity_hat
