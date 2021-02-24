import torch
import numpy as np

# 用训练好的模型进行预测并输出预测结果
save_path = './model/'
# 从模型中获取预测程序、输入数据名称列表、分类器
load_model = torch.load(save_path + 'model.pth')


# 获取数据
def get_data(sentence):
    # 读取数据字典
    with open('./data/dict.txt', 'r', encoding='utf-8') as f_data:
        dict_txt = eval(f_data.readlines()[0])
    dict_txt = dict(dict_txt)
    # 把字符串数据转换成列表数据
    keys = dict_txt.keys()
    data = []
    for s in sentence:
        # 判断是否存在未知字符
        if s not in keys:
            s = '<unk>'
        data.append(int(dict_txt[s]))
    return np.int64(data)


sentence = '兴仁县今天抢小孩没抢走，把孩子母亲捅了一刀，看见这车的注意了，真事，车牌号辽HFM055！！！！！赶紧散播！ 都别带孩子出去瞎转悠了 尤其别让老人自己带孩子出去 太危险了 注意了！！！！辽HFM055北京现代朗动，在各学校门口抢小孩！！！110已经 证实！！全市通缉！！'
tag_hat = load_model(get_data(sentence))

# 分类名称
names = ['谣言', '非谣言']
print(sentence)
print('预测结果标签为：%d， 分类为：%s' % (tag_hat, names[tag_hat]))
