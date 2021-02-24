# 源代码说明

## ```data```
目录保存了相关的训练数据和测试数据
```pku.txt```为了最原始的《人民日报》语料库
```train_dic.txt```为基于字典的分词方法——基于字典树的双向最长匹配算法使用的数据
```train_hmm.txt```为基于统计的分词方法——隐马尔科夫模型维特比算法使用的训练数据
```train_tag.txt```为词性标注的隐马尔科夫模型的训练数据
```test_segment.txt```和```test_tag.txt```为对分词和词性标注进行准确率评价时使用的数据
```ner.pkl```为训练Bi-LSTM-CRF时使用的数据(包含训练集、测试集和相关的索引)

## ```dictionary```
目录保存了基于字典的分词方法使用的字典文件
```usr.dict```为用户自定义的字典
```words.txt```为通过数据训练出的字典

## ```model```

目录保存了模型的参数

```BiLSTM-CRF.py```和```BiLSTM-CRF.pth```分别保存了```BiLSTM-CRF```模型的代码和训练好的模型的参数
```init_cut.json```和```transition_cut.json```和```emission_cut.json```分别保存的是训练好的基于统计的分词方法的隐马尔科夫模型的初始状态概率向量、状态转移概率矩阵和发射概率矩阵的内容
```init_pos.json```和```transition_pos.json```和```emission_pos.json```分别保存的是训练好的用于词性标注的隐马尔科夫模型的初始状态概率向量、状态转移概率矩阵和发射概率矩阵的内容

## ```processing```

目录保存了数据处理和模型训练的的源代码

```process.py```为处理原始语料库的代码
```makedir.py```为生成字典的代码
```hmm_train_cut.py```和```hmm_train_tag.py```为训练隐马尔科夫模型的代码
```makepkl.py```为生成命名实体识别任务的所需数据的代码
```train_ner.py```为训练```BiLSTM-CRF```模型的代码

## ```main```

```main.py```为主程序的入口，调用的其他文件如下

```segment_dic.py```为基于字典分词的模块的源代码
```segment_hmm.py```为基于统计分词的程序的源代码
```pos_tag.py```为词性标注的模块的源代码
```evaluate.py```为对分词和词性标注进行评测的源代码
```recognition_entity.py```为命名实体识别模块的源代码