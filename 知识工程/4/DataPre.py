import json
import os
import random

data_path = "./data/"
dict_path = data_path + "dict.txt"
all_data_path = data_path + "all_data.txt"


# 生成数据字典
def create_dict():
    dict_set = set()
    # 读取全部数据
    with open(all_data_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # 把数据生成一个元组
    for line in lines:
        content = line.split('\t')[-1].replace('\n', '')
        for s in content:
            dict_set.add(s)
    # 把元组转换成字典，一个字对应一个数字
    dict_list = []
    i = 0
    for s in dict_set:
        dict_list.append([s, i])
        i += 1
    # 添加未知字符
    dict_txt = dict(dict_list)
    end_dict = {"<unk>": i}
    dict_txt.update(end_dict)
    # 把这些字典保存到本地中
    with open(dict_path, 'w', encoding='utf-8') as f:
        f.write(str(dict_txt))
    print("数据字典生成完成！")


# 创建序列化表示的数据,并按照一定比例划分训练数据与验证数据
def create_data_list():
    # 在生成数据之前，首先将eval_list.txt和train_list.txt清空
    with open(os.path.join(data_path, 'eval_list.txt'), 'w', encoding='utf-8') as f_eval:
        f_eval.seek(0)
        f_eval.truncate()

    with open(os.path.join(data_path, 'train_list.txt'), 'w', encoding='utf-8') as f_train:
        f_train.seek(0)
        f_train.truncate()

    with open(os.path.join(data_path, 'dict.txt'), 'r', encoding='utf-8') as f_data:
        dict_txt = eval(f_data.readlines()[0])

    with open(os.path.join(data_path, 'all_data.txt'), 'r', encoding='utf-8') as f_data:
        lines = f_data.readlines()

    i = 0
    with open(os.path.join(data_path, 'eval_list.txt'), 'a', encoding='utf-8') as f_eval, open(
            os.path.join(data_path, 'train_list.txt'), 'a', encoding='utf-8') as f_train:
        for line in lines:
            words = line.split('\t')[-1].replace('\n', '')
            label = line.split('\t')[0]
            labs = ""
            if i % 8 == 0:
                for s in words:
                    lab = str(dict_txt[s])
                    labs = labs + lab + ','
                labs = labs[:-1]
                labs = labs + '\t' + label + '\n'
                f_eval.write(labs)
            else:
                for s in words:
                    lab = str(dict_txt[s])
                    labs = labs + lab + ','
                labs = labs[:-1]
                labs = labs + '\t' + label + '\n'
                f_train.write(labs)
            i += 1

    print("数据列表生成完成！")


# 获取字典的长度
def get_dict_len():
    with open(dict_path, 'r', encoding='utf-8') as f:
        line = eval(f.readlines()[0])
    return len(line.keys())


def DataProcess():
    # 分别为谣言数据、非谣言数据、全部数据的文件路径
    rumor_dirs = os.listdir("./data/CED_Dataset/rumor-repost/")
    non_rumor_dirs = os.listdir("./data/CED_Dataset/non-rumor-repost/")
    original_microblog = "./data/CED_Dataset/original-microblog/"

    # 谣言标签为0，非谣言标签为1
    rumor_label = "0"
    non_rumor_label = "1"

    # 分别统计谣言数据与非谣言数据的总数
    rumor_num = 0
    non_rumor_num = 0

    # 谣言与非谣言
    all_rumor_list = []
    all_non_rumor_list = []

    # 解析谣言数据
    for rumor_class_dir in rumor_dirs:
        if rumor_class_dir != '._.DS_Store' and rumor_class_dir != '.DS_Store':
            # 遍历谣言数据，并解析
            with open(original_microblog + rumor_class_dir, 'r', encoding='utf-8') as f:
                rumor_content = f.read()
            rumor_dict = json.loads(rumor_content)
            all_rumor_list.append(rumor_label + "\t" + rumor_dict["text"] + "\n")
            rumor_num += 1

    # 解析非谣言数据
    for non_rumor_class_dir in non_rumor_dirs:
        if non_rumor_class_dir != '._.DS_Store' and non_rumor_class_dir != '.DS_Store':
            with open(original_microblog + non_rumor_class_dir, 'r', encoding='utf-8') as f2:
                non_rumor_content = f2.read()
            non_rumor_dict = json.loads(non_rumor_content)
            all_non_rumor_list.append(non_rumor_label + "\t" + non_rumor_dict["text"] + "\n")
            non_rumor_num += 1

    print("谣言数据总量为：" + str(rumor_num))
    print("非谣言数据总量为：" + str(non_rumor_num))

    # 全部数据进行乱序后写入all_data.txt
    all_data_list = all_rumor_list + all_non_rumor_list
    random.shuffle(all_data_list)
    # 在生成all_data.txt之前，首先将其清空
    with open(all_data_path, 'w') as f:
        f.seek(0)
        f.truncate()
    with open(all_data_path, 'a', encoding='utf-8') as f:
        for data in all_data_list:
            f.write(data)

    # 创建数据字典，存放位置：dict.txt。在生成之前先清空dict.txt
    with open(dict_path, 'w') as f:
        f.seek(0)
        f.truncate()
    create_dict()

    # 创建数据列表，存放位置：train_list.txt eval_list.txt
    create_data_list()

    # 获取数据字典长度
    dict_dim = get_dict_len()
    return dict_dim
