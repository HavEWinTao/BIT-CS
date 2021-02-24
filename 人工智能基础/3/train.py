import numpy as np

from ElitismGa import ElitismGA


# 将训练结果写入文件
def performance(perf, filename):
    file = open(filename, "w")
    text = 'epoch  best_pop  best_loss  best_acc  avg_fitness\n'
    for e in perf:
        text += ('  '.join(list(map(str, [e['iter'], e['best_fit']['pop'], e['best_fit']['train_loss'],
                                          e['best_fit']['train_acc'], e['avg_fitness']]))) + '\n')
    file.write(text)


# 文件路径
path = './data/'

train_path = path + "train/"
x_train = np.load(train_path + "x.npy")
y_train = np.load(train_path + "y.npy")

test_path = path + "test/"
x_test = np.load(test_path + "x.npy")
y_test = np.load(test_path + "y.npy")

train_size = len(x_train)
test_size = len(x_test)

# 加载精英遗传算法类
ga = ElitismGA(
    # 测试集与训练集
    x_train=x_train[:train_size],
    y_train=y_train[:train_size],
    x_test=x_test[:test_size],
    y_test=y_test[:test_size],
    # 迭代次数
    epochs=1000
)

# 优化网络
ga.run()

# 将测试结果写入文件
performance(ga.evaluation_history, 'ElitismGA.txt')
