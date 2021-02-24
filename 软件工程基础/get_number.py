"""随机数"""
import random


def gcd(num_1, num_2):
    """最大公约数"""
    if num_1 % num_2 == 0:
        return num_2  # 公约数
    else:
        return gcd(num_2, num_1 % num_2)  # 辗转相除法


def get_number(is_int=True):
    """获取随机整数或分数"""
    if is_int:  # 获取一个整数
        return str(random.randint(1, 100))
    else:  # 获取一个分数
        fraction = ""
        molecular = random.randint(1, 10)
        denominator = random.randint(1, 10)
        if molecular == denominator:
            return get_number(is_int=False)
        common_divisor = gcd(molecular, denominator)
        if molecular < denominator:
            fraction = str(int(molecular / common_divisor)) + \
                       " / " + str(int(denominator / common_divisor))
        if molecular > denominator:
            fraction = str(int(denominator / common_divisor)) + \
                       " / " + str(int(molecular / common_divisor))
        return fraction  # 返回经过约分的真分数


def get_exponent():
    """获取指数"""
    temp = random.randint(2, 5)
    return str(temp)
