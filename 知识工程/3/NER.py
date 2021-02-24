from abc import ABC
from typing import Any

import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.nn.functional as F
from torch import nn, optim

import DataProcess
import VecPre

torch.manual_seed(1)

EMBEDDING_DIM = 50
HIDDEN_DIM = 100

wordvector = './data/ctb.50d.vec'
wordvector_out = './data/wordvector.txt'

train = './data/Train.txt'
validation = './data/Validation.txt'
test = './data/Test.txt'

train_out = './data/train_out.txt'
validation_out = './data/validation_out.txt'
test_out = './data/test_out.txt'


def MakeIndex(file):
    for line in file:
        for words in line.split():
            word = words.split('\\')[0]
            if word not in word_to_idx:
                word_to_idx[word] = len(word_to_idx)
                idx_to_word[len(word_to_idx) - 1] = word


def MakeData(filename):
    data = []
    for line in filename:
        data_line = []
        temp_word = []
        temp_tag = []
        for word in line.split():
            temp_word.append(word.split('\\')[0])
            temp_tag.append(word.split('\\')[1])
        data_line.append(temp_word)
        data_line.append(temp_tag)
        data.append(data_line)
    return data


def Getvec(index):
    word = idx_to_word[index]
    if word in vector_to_idx:
        index_word = vector_to_idx[word]
    else:
        index_word = vector_to_idx['-unknown-']
    a = word_vector[index_word].split()[1:]
    b = map(eval, a)
    a = list(b)
    return np.array(a)


def load_embedding():
    temp_numpy = np.array([Getvec(temp) for temp in range(len(word_to_idx))])
    word_vec = torch.from_numpy(temp_numpy).float()
    return word_vec


class LSTM(nn.Module, ABC):

    def __init__(self, embedding_dim, hidden_dim, vocab_size, tagset_size):
        super(LSTM, self).__init__()
        self.hidden_dim = hidden_dim
        self.word_embeddings = nn.Embedding(vocab_size, embedding_dim)
        self.word_embeddings.weight.data = self_vector
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


def make_sequence(x, dic):
    idx = [dic[index] for index in x]
    return torch.tensor(idx, dtype=torch.long).cuda()


def Train():
    running_loss = 0
    for data in train_data:
        word, tag = data
        word_list = make_sequence(word, word_to_idx)
        tag = make_sequence(tag, tag_to_idx)
        model.zero_grad()
        model.hidden = model.init_hidden()
        tag_out = model(word_list)
        loss = loss_function(tag_out, tag)
        running_loss += loss.item()
        loss.backward()
        optimizer.step()
    LOSS.append(running_loss)
    print('epoch:', epoch, 'Loss:', running_loss)


def Accuracy_V():
    right = 0
    all_rec = 0
    real_all = 0
    for data in validation_data:
        word, tag = data
        word_list = make_sequence(word, word_to_idx)
        with torch.no_grad():
            testY = model(word_list)
        y_hat = testY.max(1)[1]
        y = make_sequence(tag, tag_to_idx)
        for i in range(len(y)):
            if y_hat[i].item() == 2 or y_hat[i].item() == 1:
                all_rec += 1
                if y[i].item() == y_hat[i].item():
                    right += 1
            if y[i].item() == 2 or y[i].item() == 1:
                real_all += 1
    Precision = round(right / all_rec, 3)
    Recall = round(right / real_all, 3)
    F_1_Measure = round(2 * Precision * Recall / (Precision + Recall), 3)
    print('Validation  查准率:', Precision, ' 查全率:', Recall, ' F_1-Measure:', F_1_Measure)
    P_v.append(Precision)
    R_v.append(Recall)
    F_1_v.append(F_1_Measure)


def Accuracy_T():
    right = 0
    all_rec = 0
    real_all = 0
    for data in test_data:
        word, tag = data
        word_list = make_sequence(word, word_to_idx)
        with torch.no_grad():
            testY = model(word_list)
        y_hat = testY.max(1)[1]
        y = make_sequence(tag, tag_to_idx)
        for i in range(len(y)):
            if y_hat[i].item() == 2 or y_hat[i].item() == 1:
                all_rec += 1
                if y[i].item() == y_hat[i].item():
                    right += 1
            if y[i].item() == 2 or y[i].item() == 1:
                real_all += 1
    Precision = round(right / all_rec, 3)
    Recall = round(right / real_all, 3)
    F_1_Measure = round(2 * Precision * Recall / (Precision + Recall), 3)
    print('test  查准率:', Precision, ' 查全率:', Recall, ' F_1-Measure:', F_1_Measure)
    P_t.append(Precision)
    R_t.append(Recall)
    F_1_t.append(F_1_Measure)


def Draw():
    EPOCHS = np.array(range(1, epochs + 1))

    Loss = np.array(LOSS)

    P_v_n = np.array(P_v)
    R_v_n = np.array(R_v)
    F_1_v_n = np.array(F_1_v)

    P_t_n = np.array(P_t)
    R_t_n = np.array(R_t)
    F_1_t_n = np.array(F_1_t)

    plt.scatter(EPOCHS, Loss, marker='*', s=5)
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.show()

    plt.scatter(EPOCHS, P_v_n, marker='+', s=5, color=(1, 0, 0), label='validation')
    plt.scatter(EPOCHS, P_t_n, marker='*', s=5, color=(0, 1, 0), label='test')
    plt.xlabel('Epoch')
    plt.ylabel('Precision')
    plt.legend()
    plt.show()

    plt.scatter(EPOCHS, R_v_n, marker='+', s=5, color=(1, 0, 0), label='validation')
    plt.scatter(EPOCHS, R_t_n, marker='*', s=5, color=(0, 1, 0), label='test')
    plt.xlabel('Epoch')
    plt.ylabel('Recall')
    plt.legend()
    plt.show()

    plt.scatter(EPOCHS, F_1_v_n, marker='+', s=5, color=(1, 0, 0), label='validation')
    plt.scatter(EPOCHS, F_1_t_n, marker='*', s=5, color=(0, 1, 0), label='test')
    plt.xlabel('Epoch')
    plt.ylabel('F1-Measure')
    plt.legend()
    plt.show()

    plt.scatter(EPOCHS, P_t_n, marker='.', s=5, color=(1, 0, 0), label='Precision')
    plt.scatter(EPOCHS, R_t_n, marker='+', s=5, color=(0, 1, 0), label='Recall')
    plt.scatter(EPOCHS, F_1_t_n, marker='o', s=5, color=(0, 0, 1), label='F1-Measure')
    plt.legend()
    plt.show()


if __name__ == '__main__':
    # 分别处理训练集、验证集、测试集文件
    DataProcess.DataProcess(train, train_out)
    DataProcess.DataProcess(validation, validation_out)
    DataProcess.DataProcess(test, test_out)
    # 将原始词向量文件精简
    VecPre.VecPre(wordvector, wordvector_out)
    # 读取训练集、验证集、测试集
    train_set = open(train_out, encoding='utf-8').readlines()
    validation_set = open(validation_out, encoding='utf-8').readlines()
    test_set = open(test_out, encoding='utf-8').readlines()
    # 读取词向量
    word_vector = open(wordvector_out, encoding='utf-8').readlines()
    # 建立词向量的索引
    vector_to_idx = {}
    for i in range(len(word_vector)):
        vector_to_idx[word_vector[i].split()[0]] = len(vector_to_idx)
    # 建立词的索引
    word_to_idx = {}
    idx_to_word = {}
    MakeIndex(train_set)
    MakeIndex(validation_set)
    MakeIndex(test_set)
    # 建立单词分类的索引
    tag_to_idx = {'B': 2, 'I': 1, 'O': 0}
    # 加载自己的词向量
    self_vector = load_embedding()
    # 声明模型
    model = LSTM(EMBEDDING_DIM, HIDDEN_DIM, len(word_to_idx), len(tag_to_idx)).cuda()
    loss_function = nn.CrossEntropyLoss()
    optimizer = optim.SGD(model.parameters(), lr=0.1)
    # 组织训练集、验证集、测试集的数据格式
    train_data = MakeData(train_set)
    validation_data = MakeData(validation_set)
    test_data = MakeData(test_set)
    # 迭代次数
    epochs = 30
    # loss
    LOSS = []
    # 验证集的相关指标
    P_v = []
    R_v = []
    F_1_v = []
    # 测试集的相关指标
    P_t = []
    R_t = []
    F_1_t = []
    # 迭代
    for epoch in range(epochs):
        print('Epoch: ', end='')
        Train()  # 训练
        Accuracy_V()  # 验证
        Accuracy_T()

    Draw()  # 画图
