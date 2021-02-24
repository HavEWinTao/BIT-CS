import os

import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.nn.functional as F
from torch import nn, optim
from torch.utils.data import DataLoader, Dataset

import DataPre

data_path = "./data/"


# 定义LSTM
class LSTM(nn.Module):

    def __init__(self, embedding_dim, hidden_dim, vocab_size, tagset_size):
        super(LSTM, self).__init__()
        self.hidden_dim = hidden_dim
        self.word_embeddings = nn.Embedding(vocab_size, embedding_dim)
        self.lstm = nn.LSTM(embedding_dim, hidden_dim)
        self.hidden2tag = nn.Linear(hidden_dim, tagset_size)
        self.hidden = self.init_hidden()

    def init_hidden(self):
        return (torch.zeros(1, 1, self.hidden_dim).cuda(),
                torch.zeros(1, 1, self.hidden_dim).cuda())

    def forward(self, sentence):
        embeds = self.word_embeddings(sentence)
        lstm_out, self.hidden = self.lstm(embeds.view(len(sentence), 1, -1), self.hidden)
        tag_space = self.hidden2tag(lstm_out.view(len(sentence), -1))
        tag_scores = F.log_softmax(tag_space, dim=1)
        return tag_scores


# 定义数据读取器
class DataSet(Dataset):
    def __init__(self, data_path):
        with open(data_path, 'r') as f:
            lines = f.readlines()
        self.data = lines
        self.len = len(lines)

    def __len__(self):
        return self.len

    def __getitem__(self, index):
        return self.data[index]


def GetVec(sentence):
    a = sentence.split(',')
    b = map(eval, a)
    a = list(b)
    return torch.from_numpy(np.array(a)).long().cuda()


def Draw(costs, accs):
    X = np.array(range(1, EPOCH_NUM + 1))
    plt.xlabel("epoch", fontsize=20)
    plt.ylabel("cost/acc", fontsize=20)
    plt.plot(X, costs, color='red', label='loss')
    plt.plot(X, accs, color='green', label='acc')
    plt.legend()
    plt.grid()
    plt.show()


# 保存模型
def SaveModel():
    model_save_dir = './model/'  # 模型保存路径
    if not os.path.exists(model_save_dir):
        os.makedirs(model_save_dir)
    torch.save(model, model_save_dir + 'model.pth')
    print('训练模型保存完成！')


# 获取训练数据读取器和测试数据读取器
BATCH_SIZE = 64
train_list_path = data_path + 'train_list.txt'
eval_list_path = data_path + 'eval_list.txt'
train_reader = DataLoader(dataset=DataSet(train_list_path), batch_size=BATCH_SIZE)
eval_reader = DataLoader(dataset=DataSet(eval_list_path), batch_size=BATCH_SIZE)
EMBEDDING_DIM = 25
HIDDEN_DIM = 25
dict_dim = DataPre.DataProcess()
model = LSTM(EMBEDDING_DIM, HIDDEN_DIM, dict_dim, 2).cuda()
loss_function = nn.CrossEntropyLoss()  # 损失函数
optimizer = optim.Adam(model.parameters(), lr=1e-4)  # 优化方法

all_train_loss = []
all_train_accs = []
all_eval_loss = []
all_eval_accs = []

EPOCH_NUM = 30  # 训练轮数
for epoch in range(EPOCH_NUM):  # 开始训练
    running_loss = 0
    num = 0
    for data in train_reader:
        for i in range(len(data)):
            words = data[i].split('\t')[0]
            word = GetVec(words)
            label = eval(data[i][:-1].split('\t')[1])
            if label == 1:
                tag = torch.ones_like(word).cuda()
            else:
                tag = torch.zeros_like(word).cuda()
            model.zero_grad()
            model.hidden = model.init_hidden()
            tag_hat = model(word)
            loss = loss_function(tag_hat, tag)
            running_loss += loss.item()
            num += 1
            loss.backward()
            optimizer.step()
    all_train_loss.append(running_loss / num)

    # 进行验证
    ac = 0
    cnt = 0
    for batch_id, data in enumerate(eval_reader):
        for i in range(len(data)):
            words = data[i].split('\t')[0]
            word = GetVec(words)
            label = eval(data[i][:-1].split('\t')[1])
            tag_hat = model(word)
            tag = tag_hat.max(1)[1]
            if (sum(tag).item() / len(tag)) > 0.5 and label == 1:
                ac += 1
            if (sum(tag).item() / len(tag)) < 0.5 and label == 0:
                ac += 1
            cnt += 1
    all_eval_accs.append(ac / cnt)
    print('epoch:', epoch, ' loss:', running_loss / num, ' acc:', ac / cnt)

SaveModel()
Draw(all_train_loss, all_eval_accs)
