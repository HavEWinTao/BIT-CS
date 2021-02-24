# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22


import codecs
import torch
import torch.optim as optim
from model.BiLSTM_CRF import BiLSTM_CRF
import pickle
from torch.utils.data import Dataset
from torch.utils.data import DataLoader

torch.manual_seed(1)  # 随机种子


# Dataset类
class dataset(Dataset):
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.len = len(self.x)

    def __len__(self):
        return self.len

    def __getitem__(self, item):
        data_item = (self.x[item], self.y[item])
        return data_item


# 将用索引表示的句子转化为字
def calculate(x, y, res):
    entity = []
    for j in range(len(x)):
        if x[j] == 0 or y[j] == 0:
            continue
        if id_tag[y[j]][0] == 'B':
            entity = [id_word[x[j].item()] + '/' + id_tag[y[j]]]
        elif id_tag[y[j]][0] == 'M' and len(entity) != 0 and entity[-1].split('/')[1][1:] == id_tag[y[j]][1:]:
            entity.append(id_word[x[j].item()] + '/' + id_tag[y[j]])
        elif id_tag[y[j]][0] == 'E' and len(entity) != 0 and entity[-1].split('/')[1][1:] == id_tag[y[j]][1:]:
            entity.append(id_word[x[j].item()] + '/' + id_tag[y[j]])
            entity.append(str(j))
            res.append(entity)
            entity = []
        else:
            entity = []
    return res


# 数据文件
data = codecs.open('../data/ner.pkl', 'rb')
word_id = pickle.load(data)
id_word = pickle.load(data)
tag_id = pickle.load(data)
id_tag = pickle.load(data)
x_train = pickle.load(data)
y_train = pickle.load(data)
x_test = pickle.load(data)
y_test = pickle.load(data)

print("train len:", len(x_train))
print("test len:", len(x_test))
# 加载训练集和测试集
train = dataset(x_train, y_train)
test = dataset(x_test, y_test)
# 将训练集和测试集组织成Dataloader形式
train_loader = DataLoader(
    dataset=train,
    batch_size=1,
    shuffle=False
)
test_loader = DataLoader(
    dataset=test,
    batch_size=1,
    shuffle=False
)
# 开始和结束的标签
START_TAG = "<START>"
STOP_TAG = "<STOP>"
tag_id[START_TAG] = len(tag_id)
tag_id[STOP_TAG] = len(tag_id)
# 超参数
EMBEDDING_DIM = 100
HIDDEN_DIM = 200
"""
迭代次数
因算力限制，仅将迭代次数设置为2
"""
epochs = 2
# 定义模型
model = BiLSTM_CRF(len(word_id) + 1, tag_id, EMBEDDING_DIM, HIDDEN_DIM).cuda()
# 定义优化器
optimizer = optim.SGD(model.parameters(), lr=0.01, weight_decay=1e-3)
# 训练
for epoch in range(epochs):
    for step, (sentence, tags) in enumerate(train_loader):
        model.zero_grad()
        # 生成输入数据
        sentence = sentence[0].long().cuda()
        tags = tags.long()[0].cuda()
        # 计算损失
        loss = model.neg_log_likelihood(sentence, tags)
        # 反向传播
        loss.backward()
        optimizer.step()
    entity_hat = []
    entities = []
    # 计算准确率、召回率、F1
    for step, (sentence, tags) in enumerate(test_loader):  # 60代表一个句子,为word2id 和 tag2id
        sentence = sentence[0].long().cuda()
        tags = tags.long()[0].cuda()
        _, predict = model(sentence)
        tags = tags.cpu().numpy().tolist()
        entity_hat = calculate(sentence, predict, entity_hat)
        entities = calculate(sentence, tags, entities)
    same = [i for i in entity_hat if i in entities]
    if len(same) != 0:
        P = float(len(same)) / len(entity_hat)
        R = float(len(same)) / len(entities)
        print("准确率：", round(P, 3), " ", end='')
        print("召回率：", round(R, 3), " ", end='')
        print("F1：", round(((2 * P * R) / (P + R)), 3))
    else:
        print("准确率：", 0)
# 保存模型
path = "../model/BiLSTM_CRF.pth"
torch.save(model, path)
print("BiLSTM_CRF model has been saved")
