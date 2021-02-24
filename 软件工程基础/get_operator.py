"""随机数"""
import random

operator = ['+', '-', '*', '/', '^']


def get_operator(is_int=False):
    """随机获取运算符"""
    index = random.random()
    if is_int:  # 整数运算的运算符
        if index > 0.7:
            return operator[0]
        elif index > 0.4:
            return operator[1]
        elif index > 0.2:
            return operator[2]
        elif index > 0.05:
            return operator[3]
        else:
            return operator[4]
    else:  # 分数运算的运算符
        if index > 0.65:
            return operator[0]
        elif index > 0.3:
            return operator[1]
        elif index > 0.15:
            return operator[2]
        elif index > 0.05:
            return operator[3]
        else:
            return operator[4]
