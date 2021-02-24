# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

import time

from AutoCorrect import AtuoCorrect
from eval import evaluator

if __name__ == '__main__':
    print("加载模型中，请稍等~~~")
    # 加载评测器和更正器，以便提高后续速度
    Evaluator = evaluator()
    Corrector = AtuoCorrect()
    print("输入help查看帮助")
    while True:
        # 输入request，根据request来执行和后续操作
        request = input("requests：")
        if request == "help":
            print("句子纠错——correct")
            print("准确率评估——eval")
        elif request == "correct":
            sentence = input("请输入要纠错的句子：")
            start = time.time()  # 记录时间
            # 对句子进行更正
            correct_sent = Corrector.correct(sentence,True)
            print("输入的句子：" + sentence)
            print("纠错后的句子：" + correct_sent)
            end = time.time()
            print("cost：", round(end - start, 3), "s")
        elif request == "eval":
            # 准确率和花费时间
            acc, cost = Evaluator.eval()
            print("准确率：", acc)
            print("cost：", cost, "s")
        else:  # 错误地输入
            print("请输入正确的指令")
