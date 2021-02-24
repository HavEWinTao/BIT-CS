# coding=utf-8
# Author:fan hongtao
# Date:2020-11-22

import segment_dic
import segment_hmm
import pos_tag
import evaluate
import ner

if __name__ == '__main__':
    # 加载分词、词性标注、命名实体识别的模型
    Segment_dic = segment_dic.segment()
    Segment_hmm = segment_hmm.segment()
    Pos_tag = pos_tag.pos()
    Evaluator = evaluate.eval()
    Recognition_entity = ner.recognition_entity()
    # 根据命令执行操作
    print("输入help查看帮助")
    while True:
        request = input("request：")
        if request == "help":
            print("字典分词——segment dic")
            print("增加用户词汇——add")
            print("统计分词——segment hmm")
            print("序列标注——pos")
            print("准确率评测——eval")
            print("命名实体识别——ner")
        elif request == "segment dic":
            sent = input("请输入要分词的句子:")
            ans = Segment_dic.bi_segment(sent)
            print("分词结果为：")
            print(ans)
        elif request == "add":
            print("注意：用户字典仅对基于词典规则分词的算法有效")
            Segment_dic.add_word()
        elif request == "segment hmm":
            sent = input("请输入要分词的句子:")
            ans = Segment_hmm.hmm_segment(sent)
            print("分词结果为：")
            print(ans)
        elif request == "pos":
            sent = input("请输入要序列标注的句子:")
            sent, tag = Pos_tag.pos_tag(sent)
            print("分词结果为：")
            print(sent)
            print("序列标注：")
            print(tag)
        elif request == "eval":
            print("开始测试模型的准确率")
            Evaluator.evaluate()
        elif request == 'ner':
            sent = input("请输入识别命名实体的句子:")
            ans = Recognition_entity.ner(sent)
            print("句子中包含的命名实体有：")
            print(ans)
        else:
            print("请输入正确的指令")
