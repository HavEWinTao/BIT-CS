"""计算结果"""
from solve import solve


def check(problem, problemlist):
    """检查是否有重复的题"""
    flag = 1
    cur_result = solve(problem.split(" "))
    if cur_result == "DivZeroError":
        return False
    for item in problemlist:
        result = solve(item.split(" "))
        if result == cur_result:
            flag = 0
            break
    if flag:
        return True
    else:
        return False
