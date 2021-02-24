import matplotlib.pyplot as plt
import numpy as np

import GetPair
import MakeDir
import Const

theta = np.zeros(shape=(1, Const.Length * 3))
onehot = np.zeros(shape=(Const.Length * 3, 1))

# 返回one-hot向量
def OneHot(fir_word, sec_word, thi_word):
    onehot_vec = np.zeros(shape=(Const.Length * 3, 1))
    if fir_word in token_index:
        onehot_vec[token_index.get(fir_word)] = 1
    else:
        onehot_vec[Const.Length - 1] = 1
    if sec_word in token_index:
        onehot_vec[Const.Length + token_index.get(sec_word)] = 1
    else:
        onehot_vec[Const.Length * 2 - 1] = 1
    if thi_word in token_index:
        onehot_vec[Const.Length * 2 + token_index.get(thi_word)] = 1
    else:
        onehot_vec[Const.Length * 3 - 1] = 1
    return onehot_vec

# Sigmoid函数
def Sigmoid(onehot_vec):
    z = np.dot(theta, onehot_vec)
    s = 1 / (1 + np.exp(-z))
    return s

# loss
def cost_function(onehot_vec, y):
    y_hat = Sigmoid(onehot_vec)
    return np.sum(-y * np.log(y_hat) - (1 - y) * np.log(1 - y_hat))

# 判断单词是否为命名实体
def GetY(word):
    if word.split()[1] == Const.target:
        return 1
    else:
        return 0

# 训练theta
def Predict():
    global theta
    global onehot

    for i in range(1, len(train) - 1):
        fir_word = train[i - 1].split()[0]
        sec_word = train[i].split()[0]
        thi_word = train[i + 1].split()[0]
        onehot = OneHot(fir_word, sec_word, thi_word)
        y = GetY(train[i])
        y_hat = Sigmoid(onehot)
        theta = theta + Const.alpha * (y - y_hat) * onehot.T

# 验证并计算准确率
def Accuracy():
    global onehot

    right = 0
    all_rec = 0
    real_all = 0
    LOSS = 0

    for i in range(1, len(test) - 1):
        fir_word = test[i - 1].split()[0]
        sec_word = test[i].split()[0]
        thi_word = test[i + 1].split()[0]
        onehot = OneHot(fir_word, sec_word, thi_word)
        y_hat = Sigmoid(onehot)
        if y_hat > 0.5:
            all_rec += 1
        if test[i].split()[1] == Const.target:
            real_all += 1
        if y_hat > 0.5 and test[i].split()[1] == Const.target:
            right += 1
        y = GetY(test[i])
        LOSS += cost_function(onehot, y)
    P = round(right / all_rec, 3)
    R = round(right / real_all, 3)
    F_1 = round(2 * P * R / (P + R), 3)
    LOSS = round(LOSS, 3)
    print('loss:', LOSS, '查准率:', P, ' 查全率:', R, ' F_1-Measure:', F_1)
    loss.append(LOSS)
    p.append(P)
    r.append(R)
    f_1.append(F_1)
    return P, R, F_1

# 将每一轮迭代的评价指标和loss绘制散点图
def Draw():
    temp = list(range(len(loss)))
    epochs = np.array(temp)
    LOSS = np.array(loss)
    P = np.array(p)
    R = np.array(r)
    F_1 = np.array(f_1)
    plt.scatter(epochs, LOSS, marker='.', s=2)
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.show()
    plt.scatter(epochs, P, marker='.', s=2)
    plt.xlabel('Epoch')
    plt.ylabel('P')
    plt.show()
    plt.scatter(epochs, R, marker='.', s=2)
    plt.xlabel('Epoch')
    plt.ylabel('R')
    plt.show()
    plt.scatter(epochs, F_1, marker='.', s=2)
    plt.xlabel('Epoch')
    plt.ylabel('F_1-Measure')
    plt.show()
    plt.scatter(epochs, P, marker='.', s=2, color=(1, 0, 0), label='P')
    plt.scatter(epochs, R, marker='.', s=2, color=(0, 1, 0), label='R')
    plt.scatter(epochs, F_1, marker='.', s=2, color=(0, 0, 1), label='F_1-Measure')
    plt.show()


if __name__ == '__main__':
    GetPair.GetPair(Const.train_set, Const.train_out)
    GetPair.GetPair(Const.test_set, Const.test_out)
    GetPair.GetPair(Const.Validation_set, Const.Validation_out)
    token_index = MakeDir.MakeDir()
    train = open(Const.train_out, encoding='utf-8').readlines()
    test = open(Const.test_out, encoding='utf-8').readlines()
    loss = []
    p = []
    r = []
    f_1 = []
    # 多次迭代
    for epoch in range(Const.max_lte):
        print('Epoch:', epoch, ' ', end='')
        Predict()
        Accuracy()
    Draw()
