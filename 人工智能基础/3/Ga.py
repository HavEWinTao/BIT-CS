from abc import abstractmethod

import numpy as np
from keras.layers import Conv2D, Dense, Flatten
from keras.models import Sequential


class GA:
    def __init__(self, x_train, y_train, x_test, y_test, epochs):
        # 初始化参数
        self.x_train = x_train
        self.y_train = y_train
        self.x_test = x_test
        self.y_test = y_test
        self.pop_size = 20  # 种群大小
        # 交叉、变异概率
        self.r_mutation = 0.1
        self.p_crossover = 0
        self.p_mutation = 0.2
        self.epochs = epochs
        self.min_fitness = 0.95  # 适应度
        self.elite_num = 2
        self.mating_pool_size = 4
        self.batch_size = 32

        self.chroms = []  # 保存网络
        self.evaluation_history = []  # 进化历史
        self.stddev = 0.5  # 样本标准偏差
        self.loss_func = 'mse'  # loss_function
        self.metrics = ['accuracy']  # evaluation_function

    @property
    def cur_iter(self):
        return len(self.evaluation_history)

    # 将数据集顺序打乱
    def shuffle_batch(self):
        series = list(range(len(self.x_train)))
        np.random.shuffle(series)
        return series

    # 初始化
    def init(self):
        for i in range(self.pop_size):
            # 神经网络模型的结构
            model = Sequential()
            model.add(Conv2D(8, (3, 3), activation='relu', use_bias=False, input_shape=(15, 15, 2)))
            model.add(Conv2D(32, (3, 3), activation='relu', use_bias=False))
            model.add(Conv2D(128, (3, 3), activation='relu', use_bias=False))
            model.add(Conv2D(128, (1, 1), activation='relu', use_bias=False))
            model.add(Flatten())
            model.add(Dense(128, activation='relu', use_bias=False))
            model.add(Dense(64, activation='relu', use_bias=False))
            model.add(Dense(1, use_bias=False))
            self.chroms.append(model)
        print('network initialization finished')

    # 评估
    def evaluation(self, _X, _y, _is_batch=True):
        cur_evaluation = []
        for i in range(self.pop_size):
            model = self.chroms[i]
            model.compile(loss=self.loss_func, metrics=self.metrics, optimizer='adam')
            train_loss, train_acc = model.evaluate(_X, _y, verbose=0)
            # 保存评估历史
            if not _is_batch:
                test_loss, test_acc = model.evaluate(self.x_test, self.y_test, verbose=0)
                cur_evaluation.append({
                    'pop': i,
                    'train_loss': round(train_loss, 4),
                    'train_acc': round(train_acc, 4),
                    'test_loss': round(test_loss, 4),
                    'test_acc': round(test_acc, 4),
                })
            else:
                cur_evaluation.append({
                    'pop': i,
                    'train_loss': round(train_loss, 4),
                    'train_acc': round(train_acc, 4),
                })
        best_fit = sorted(cur_evaluation, key=lambda x: x['train_acc'])[-1]
        self.evaluation_history.append({
            'iter': self.cur_iter + 1,
            'best_fit': best_fit,
            'avg_fitness': np.mean([e['train_acc'] for e in cur_evaluation]).round(4),
            'evaluation': cur_evaluation,
        })
        print('\nIter: {}'.format(self.evaluation_history[-1]['iter']))
        print('Best_fit: {}, avg_fitness: {:.4f}'.format(self.evaluation_history[-1]['best_fit'],
                                                         self.evaluation_history[-1]['avg_fitness']))

    # 选择算法
    def roulette_wheel_selection(self):
        sorted_evaluation = sorted(self.evaluation_history[-1]['evaluation'], key=lambda x: x['train_acc'])
        cum_acc = np.array([e['train_acc'] for e in sorted_evaluation]).cumsum()
        extra_evaluation = [{'pop': e['pop'], 'train_acc': e['train_acc'], 'cum_acc': acc}
                            for e, acc in zip(sorted_evaluation, cum_acc)]
        rand = np.random.rand() * cum_acc[-1]
        for e in extra_evaluation:
            if rand < e['cum_acc']:
                return e['pop']
        return extra_evaluation[-1]['pop']

    # 供外部调用的接口
    @abstractmethod
    def run(self):
        raise NotImplementedError('Please finish this function')

    # 选择
    @abstractmethod
    def select(self):
        raise NotImplementedError('Please finish this function')

    # 交叉
    @abstractmethod
    def crossover(self, _selected_pop):
        raise NotImplementedError('Please finish this function')

    # 变异
    @abstractmethod
    def mutate(self, _selected_pop):
        raise NotImplementedError('Please finish this function')

    # 替换
    @abstractmethod
    def replace(self, _child):
        raise NotImplementedError('Please finish this function')
