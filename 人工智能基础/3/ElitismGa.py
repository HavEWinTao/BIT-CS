import numpy as np
from keras.layers import Conv2D, Dense
from keras.models import Sequential, clone_model

from Ga import GA


class ElitismGA(GA):
    def run(self):
        print('Elitism GA is running...')
        self.init()
        # 执行遗传算法
        while 1:
            # 打乱种群
            series = self.shuffle_batch()
            for i in range(self.batch_size, len(series) + 1, self.batch_size):
                index = series[i - self.batch_size:i]
                X_batch = self.x_train[index]
                y_batch = self.y_train[index]
                # 评估适应率
                if i + self.batch_size > len(series):
                    self.evaluation(X_batch, y_batch, False)
                else:
                    self.evaluation(X_batch, y_batch)
                # 选择
                self.select()
                # 达到最大遗传代数
                if self.cur_iter >= self.epochs:
                    print("Maximum iterations ", self.epochs, " reached")
                    return
                # 达到最小适应率
                if self.evaluation_history[-1]['best_fit']['train_acc'] >= self.min_fitness:
                    print("Minimum fitness ", self.min_fitness, " reached")
                    return

    # 选择
    def select(self):
        sorted_evaluation = sorted(self.evaluation_history[-1]['evaluation'], key=lambda x: x['train_acc'])
        elites = [e['pop'] for e in sorted_evaluation[-self.elite_num:]]
        print("Elites: ", elites)
        children = [self.chroms[i] for i in elites]
        mating_pool = np.array([self.roulette_wheel_selection() for _ in range(self.mating_pool_size)])
        pairs = []  # 双亲
        while len(children) < self.pop_size:
            pair = [np.random.choice(mating_pool) for _ in range(2)]
            pairs.append(pair)
            children.append(self.crossover(pair))
        print("Pairs: ", pairs)
        print('Crossover finished.')
        self.replace(children)
        for i in range(self.elite_num, self.pop_size):
            if np.random.rand() < self.p_mutation:
                mutated_child = self.mutate(i)
                del self.chroms[i]
                self.chroms.insert(i, mutated_child)

    # 交叉
    def crossover(self, selected_pop):
        if selected_pop[0] == selected_pop[1]:
            return clone_model(self.chroms[selected_pop[0]])
        child = Sequential()
        chrom1 = clone_model(self.chroms[selected_pop[0]])
        chrom2 = clone_model(self.chroms[selected_pop[1]])
        chrom1_layers = chrom1.layers
        chrom2_layers = chrom2.layers
        # 交换某一层的参数
        for i in range(len(chrom1_layers)):
            layer1 = chrom1_layers[i]
            layer2 = chrom2_layers[i]
            if type(layer1) is Conv2D:
                child.add(layer1 if np.random.rand() < 0.5 else layer2)
            elif type(layer1) is Dense:
                weights1 = layer1.get_weights()[0]
                weights2 = layer2.get_weights()[0]
                rand1 = np.random.randint(0, 2, weights1.shape[1])
                rand2 = 1 - rand1
                layer1.set_weights([weights1 * rand1 + weights2 * rand2])
                child.add(layer1)
            else:
                child.add(layer1)
        del chrom1
        del chrom2
        return child

    # 变异
    def mutate(self, selected_pop):
        child = Sequential()
        chrom = clone_model(self.chroms[selected_pop])
        chrom_layers = chrom.layers
        for layer in chrom_layers:
            # 更改参数
            if type(layer) is Conv2D:
                if np.random.rand() < self.r_mutation:
                    weights = layer.get_weights()[0]
                    layer.set_weights([weights + np.random.normal(0, self.stddev, weights.shape)])
                child.add(layer)
            elif type(layer) is Dense:
                weights = layer.get_weights()[0]
                rand = np.where(np.random.rand(weights.shape[1]) < self.r_mutation, 1, 0)
                layer.set_weights([weights + rand * np.random.normal(0, self.stddev, weights.shape)])
                child.add(layer)
            else:
                child.add(layer)
        del chrom
        print("Mutate ", selected_pop)
        return child

    # 替换
    def replace(self, _child):
        self.chroms[:] = _child
        print('Replacement finished.')
