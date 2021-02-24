import matplotlib.pyplot as plt
import numpy as np
import torch
from torch.nn import init

import Const
import VecPre


def MakeIndex():
    dictionary = {}
    for i in range(len(WordVec)):
        dictionary[WordVec[i].split()[0]] = i
    return dictionary


def BatchX(tag):
    if tag in index:
        temp = index[tag]
    else:
        temp = index['-unknown-']
    a = WordVec[temp].split()[1:]
    b = map(eval, a)
    a = list(b)
    return np.array(a)


def WordVector(tag, opt):
    fir = torch.tensor(BatchX(opt[tag - 1].split()[0]))
    sec = torch.tensor(BatchX(opt[tag].split()[0]))
    thi = torch.tensor(BatchX(opt[tag + 1].split()[0]))
    return torch.cat([torch.cat([fir, sec], dim=0), thi], dim=0).numpy()


def encodeY(tag):
    if tag == 'B':
        return 2
    elif tag == 'I':
        return 1
    else:
        return 0


def Train():
    # cnt = 0
    loss_sum = 0
    for start in range(1, len(train) - 1, Const.BatchSize):
        end = min(start + Const.BatchSize, len(train) - 1)
        trX = torch.tensor([WordVector(i, train) for i in range(start, end)]).float()
        trY = torch.tensor([encodeY(train[i].split()[1]) for i in range(start, end)]).long()
        y_pred = model(trX)
        LOSS = loss_function(y_pred, trY)
        loss_sum += LOSS.item()
        optimizer.zero_grad()
        LOSS.backward()
        optimizer.step()
        # cnt += 1
        # if cnt == 10:
        #     cnt = 0
    print('Epoch:', epoch, 'loss:', loss_sum)
    loss.append(loss_sum)
    # loss_sum = 0
    Accuracy()


def Accuracy():
    right = 0
    all_rec = 0
    real_all = 0

    for start in range(1, len(test) - 1, Const.BatchSize):
        end = min(start + Const.BatchSize, len(test) - 1)
        testX = torch.tensor([WordVector(i, test) for i in range(start, end)]).float()
        testY = model(testX)
        y_hat = testY.max(1)[1]
        y = torch.tensor([encodeY(test[i].split()[1]) for i in range(start, end)])
        for i in range(y.shape[0]):
            if y_hat[i].item() == 2 or y_hat[i].item() == 1:
                all_rec += 1
                if y[i] == y_hat[i]:
                    right += 1
            if y[i].item() == 2 or y[i].item() == 1:
                real_all += 1
    P = round(right / all_rec, 3)
    R = round(right / real_all, 3)
    F_1 = round(2 * P * R / (P + R), 3)
    print('查准率:', P, ' 查全率:', R, ' F_1-Measure:', F_1)
    p.append(P)
    r.append(R)
    f_1.append(F_1)


def Draw():
    EPOCHS = np.array(range(1, Const.epochs + 1))
    Loss = np.array(loss)
    P_n = np.array(p)
    R_n = np.array(r)
    F_1_n = np.array(f_1)
    plt.scatter(EPOCHS, Loss, marker='.', s=5)
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.show()
    plt.scatter(EPOCHS, P_n, marker='.', s=5)
    plt.xlabel('Epoch')
    plt.ylabel('P')
    plt.show()
    plt.scatter(EPOCHS, R_n, marker='.', s=5)
    plt.xlabel('Epoch')
    plt.ylabel('R')
    plt.show()
    plt.scatter(EPOCHS, F_1_n, marker='.', s=5)
    plt.xlabel('Epoch')
    plt.ylabel('F_1-Measure')
    plt.show()
    plt.scatter(EPOCHS, P_n, marker='.', s=5, color=(1, 0, 0), label='P')
    plt.scatter(EPOCHS, R_n, marker='+', s=5, color=(0, 1, 0), label='R')
    plt.scatter(EPOCHS, F_1_n, marker='o', s=5, color=(0, 0, 1), label='F_1-Measure')

    plt.show()


if __name__ == '__main__':

    VecPre.VecPre(Const.wordvec, Const.wordvecpre)

    WordVec = open(Const.wordvecpre, encoding='utf-8').readlines()
    train = open(Const.train_out, encoding='utf-8').readlines()
    test = open(Const.test_out, encoding='utf-8').readlines()

    index = MakeIndex()

    model = torch.nn.Sequential(
        torch.nn.Linear(150, 25, bias=False),
        # torch.nn.ReLU(),
        torch.nn.Linear(25, 3, bias=False)
    )
    for params in model.parameters():
        init.normal_(params, mean=0, std=0.01)
    loss_function = torch.nn.CrossEntropyLoss()
    optimizer = torch.optim.SGD(model.parameters(), lr=0.1)

    loss = []
    p = []
    r = []
    f_1 = []

    for epoch in range(1, 1 + Const.epochs):
        Train()

    Draw()
