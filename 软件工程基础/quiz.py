"""计算结果"""
from solve import solve


def quiz(show_style, is_int=True):
    """测验"""
    print("注意！")
    print("若结果为分数不需转换为小数。")
    if is_int:
        path = "problem_int.txt"
    else:
        path = "problem_fraction.txt"
    file = open(path, "r", encoding="utf8")
    problems = file.readlines()
    total = len(problems)
    correct = 0
    for problem in problems:
        problem = problem.strip('\n')
        if show_style == 1:
            print(problem.replace("^", "**"))
        else:
            print(problem)
        correct_answer = solve(problem.split(" "))
        while True:
            answer = input("请输入答案： ")
            if answer != "":
                break
        if answer == str(correct_answer):
            correct += 1
            print("此题正确")
        else:
            print("此题错误")
    file.close()
    print("测试结束！")
    print("你答对的题目数量为：", correct, "/", total)
