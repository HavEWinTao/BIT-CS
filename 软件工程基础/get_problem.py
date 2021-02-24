"""随机数"""
import random

from check import check
from get_number import get_number, get_exponent
from get_operator import get_operator


def generate_problem(num, is_int=True):
    """生成问题"""
    problemlist = []
    i = 0
    # 循环生成num数量的问题
    while i < num:
        if not is_int:
            temp_num = random.randint(2, 5)
        else:
            temp_num = random.randint(2, 10)
        bracket_num = 0
        distance = 0
        problem = get_number(is_int=is_int)
        for j in range(temp_num):
            temp = get_operator()
            problem += " "
            problem += temp
            if temp == "^":  # 指数运算修正
                correct = problem.split()
                correct[-2] = correct[-2][0]
                problem = ""
                for item in correct:
                    problem += item
                    problem += " "
                problem += get_exponent()
            else:
                if (temp_num - j) > 2:
                    if random.random() > 0.8:
                        problem += " "
                        problem += "("
                        bracket_num += 1
                        distance = 0
                problem += " "
                problem += get_number(is_int=is_int)
            if distance > 1:
                if bracket_num != 0:
                    if random.random() > 0.7:
                        problem += " "
                        problem += ")"
                        bracket_num -= 1
            distance += 1
        for j in range(bracket_num):
            problem += " "
            problem += ")"

        if check(problem, problemlist):
            problemlist.append(problem)
            i += 1
    # 将问题保存到文件中
    if is_int:
        path = "problem_int.txt"
    else:
        path = "problem_fraction.txt"
    file = open(path, "w", encoding="utf8")
    for problem in problemlist:
        file.write(problem)
        file.write('\n')
    file.close()

    print("\ngenerate problems finsih!")
    print("all the problems have been saved in ", path)
