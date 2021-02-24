import os

import numpy as np

# 训练集和测试集
modes = ["train", "test"]
for mode in modes:
    # 文件路径
    path = "./data/" + mode
    table = path + "/table"
    label = path + "/score"
    # 文件数量
    tables = os.listdir(table)
    num = len(tables)

    x = np.zeros(shape=(len(tables), 15, 15, 2))
    y = np.zeros(len(tables))
    for i in range(num):
        square_state = np.zeros(shape=(2, 15, 15))
        # 棋盘特征提取
        state = np.load(table + "/" + str(i) + ".npy")
        black = np.where(state == 1)
        white = np.where(state == 2)
        square_state[0][black] = 1.0
        square_state[1][white] = 1.0
        square_state = square_state.transpose(1, 2, 0)
        x[i] = square_state
        # 分数
        score = open(label + "/" + str(i) + ".txt").readline()
        score = int(score)
        # 对分数取以10为底的对数
        if score > 0:
            score = np.log10(score)
        elif score == 0:
            score = 0
        else:
            score = -np.log10(-score)
        score = np.tanh(score)
        y[i] = score
    # 以array格式保存
    np.save(path + "/x", x)
    np.save(path + "/y", y)
