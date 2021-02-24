import re
from math import inf

VACANT = 0  # 空
BLACK = 1  # 黑子
WHITE = 2  # 白子


# 如果是电脑先手,先手棋为白色


# 判断搜索空间
def search_space(table, sequence, stat=None):
    # table: 当前棋盘
    # sequence: 当前棋盘上棋子的落子顺序序列
    """根据当前棋盘上棋子的落子顺序序列来计算下一步的搜索空间
    当某个空点在以其为中心的5*5的方格范围内有子的情况下，才将其加入搜索空间
    越靠近最近几步的落子点的空点，越有可能是最优解
    因此在扩展搜索空间时，按sequence中顺序的相反顺序搜索"""
    n = len(table)  # 棋盘大小
    # 根据落子序列和棋盘状态获取搜索空间
    if not stat:
        space = []  # 搜索空间
        for i0, j0 in sequence[-1::-1]:
            # delta_i,delta_j为与当前i0,j0的坐标差
            for delta_i in [0, -1, 1, -2, 2]:
                for delta_j in [0, -1, 1, -2, 2]:
                    # i1,j1为当前坐标
                    i1, j1 = i0 + delta_i, j0 + delta_j
                    # 将i1,j1加入搜索空间的条件
                    if 0 <= i1 < n and 0 <= j1 < n and table[i1][j1] == VACANT and (i1, j1) not in space:
                        space.append((i1, j1))
        return space  # 计算出的下一步的搜索空间
    # stat的内容是i,j,当i,j处落子后再次对搜索空间拓展
    else:
        i, j = stat
        for delta_i in [-2, 2, -1, 1, 0]:
            for delta_j in [-2, 2, -1, 1, 0]:
                i1, j1 = i + delta_i, j + delta_j
                if 0 <= i1 < n and 0 <= j1 < n and table[i1][j1] == VACANT:
                    if (i1, j1) in table:
                        sequence.remove((i1, j1))
                    sequence = [(i1, j1)] + sequence
        return sequence


# 搜索落子点
def search(table, sequence, depth):
    n = len(table)  # 棋盘大小
    if not sequence:  # 电脑先手下中心，符合常识
        return n // 2, n // 2
    # alpha为已知的最大值
    # beta为已知的最小值
    # 因为还没搜索不知道是多少,初始化为-inf和inf
    alpha, beta = -inf, inf
    # 对电脑越有利,分数越大
    # 对玩家越有利,分数越小
    # 初值设为-inf
    max_ = -inf
    max_i, max_j = 0, 0  # 返回的落子位置
    space = search_space(table, sequence)  # 搜索空间
    # 遍历搜索空间内的每个点
    for (i, j) in space:
        if table[i][j] == VACANT:
            # 假定下棋
            table[i][j] = WHITE
            result = judge(table)
            # 白棋落此位置会获胜
            if result and result[-1] == WHITE:
                table[i][j] = VACANT
                return i, j
            # i,j落子后的space
            new_space = search_space(table, space, stat=(i, j))
            # 搜索并得到分数
            # 电脑当前步最大的分
            score = min_search(table, alpha, beta, depth=depth, space=new_space)
            # 撤销下棋
            table[i][j] = VACANT
            # 判断得分并记录得分最高的分数和对应位置
            if score > max_:
                max_ = score
                max_i, max_j = i, j
            # 如果当前score比beta的值大,直接返回坐标
            if max_ >= beta:
                return max_i, max_j
            # 修改alpha值
            if max_ > alpha:
                alpha = max_
    return max_i, max_j  # 返回落子位置


# 搜索极大值。
def max_search(table, alpha, beta, depth, space):
    # 搜索结束
    if depth == 0:
        return evaluate(table)
    # 电脑的最大分数
    max_ = -inf
    for (i, j) in space:
        if table[i][j] == VACANT:
            table[i][j] = WHITE  # 电脑落子
            result = judge(table)  # 判断是否五子
            if result and result[-1] == WHITE:
                table[i][j] = VACANT
                return 10000000
                # 不可以return inf
                # 因为inf > inf为False，因此若return inf则在上一层函数中以下代码运行可能出现问题
            new_space = search_space(table, space, stat=(i, j))
            # 玩家的最小分数
            score = min_search(table, alpha, beta, depth=depth - 1, space=new_space)
            table[i][j] = VACANT  # 撤销落子
            # alpha、beta剪枝
            if score > max_:
                max_ = score
            if max_ >= beta:
                return max_
            if max_ > alpha:
                alpha = max_
    return max_


# 搜索极小值
def min_search(table, alpha, beta, depth, space):
    if depth == 0:  # 深搜结束
        return evaluate(table)
    # 玩家的最小分数
    # 初值设为inf
    min_ = inf
    for (i, j) in space:
        if table[i][j] == VACANT:
            # 假设玩家落子
            table[i][j] = BLACK
            result = judge(table)  # 判断是否五子
            if result and result[-1] == BLACK:
                table[i][j] = VACANT
                return -10000000
            new_space = search_space(table, space, stat=(i, j))
            # 搜索电脑的最大值
            score = max_search(table, alpha, beta, depth=depth - 1, space=new_space)
            if score < min_:
                min_ = score
            table[i][j] = VACANT  # 撤销落子
            # alpha、beta剪枝
            if min_ <= alpha:
                return min_
            if min_ < beta:
                beta = min_
    return min_


patterns = {}  # 各种状态对应的分数
# x表示空
for color in [BLACK, WHITE]:  # 对黑棋白棋进行遍历
    opponent_color = BLACK if color == WHITE else WHITE
    factor = 1 if color == WHITE else -1  # 加分与减分
    # x110 011x
    patterns[str(VACANT) + str(color) * 2 + str(opponent_color)] = factor
    patterns[str(opponent_color) + str(color) * 2 + str(VACANT)] = factor
    # x11x
    patterns[str(VACANT) + str(color) * 2 + str(VACANT)] = factor * 5
    # x1110 0111x
    patterns[str(VACANT) + str(color) * 3 + str(opponent_color)] = factor * 10
    patterns[str(opponent_color) + str(color) * 3 + str(VACANT)] = factor * 10
    # x11x1x x1x11x
    patterns[str(VACANT) + str(color) * 2 + str(VACANT) + str(color) + str(VACANT)] = factor * 100
    patterns[str(VACANT) + str(color) + str(VACANT) + str(color) * 2 + str(VACANT)] = factor * 100
    # x111x
    patterns[str(VACANT) + str(color) * 3 + str(VACANT)] = factor * 100
    # x11110 01111x
    patterns[str(VACANT) + str(color) * 4 + str(opponent_color)] = factor * 1000
    patterns[str(opponent_color) + str(color) * 4 + str(VACANT)] = factor * 1000
    # x1111x
    patterns[str(VACANT) + str(color) * 4 + str(VACANT)] = factor * 10000
    # 111111 x11111x
    patterns[str(color) * 5] = factor * 1000000
    patterns[str(VACANT) + str(color) * 5 + str(VACANT)] = factor * 1000000
    # 此行代码只是为了防止人类玩家即将连到5个时AI不去拦截（因为拦截也没用）的尴尬局面（可删，不影响胜负）


# 评估函数
def evaluate(table):
    n = len(table)  # 棋盘大小
    score = 0  # 得分
    strings = []
    # 遍历每一行
    for i in range(n):
        strings.append(''.join([str(j) for j in table[i]]))
    # 遍历每一列
    for j in range(n):
        strings.append(''.join([str(table[i][j]) for i in range(n)]))
    # 右斜线
    for k in range(2 * n - 1):
        if k < n:
            strings.append(''.join([str(table[k - i][i]) for i in range(k + 1)]))
        else:
            strings.append(''.join([str(table[n - 1 - i][k - n + 1 + i]) for i in range(2 * n - k - 1)]))
    # 左斜线
    for k in range(2 * n - 1):
        if k < n:
            strings.append(''.join([str(table[n - 1 - k + i][i]) for i in range(k + 1)]))
        else:
            strings.append(''.join([str(table[i][k - n + 1 + i]) for i in range(2 * n - k - 1)]))
    # 根据棋局状态判断得分
    for string in strings:
        for pattern in patterns:
            # 与设定的评估函数值比较
            # 计算出score
            score += patterns[pattern] * len(re.findall(pattern, string))
    return score


#  判断是否已分出胜负
def judge(table):
    # table: n*n列表，用于存储棋盘上各位置的信息，BLACK表示黑子，WHITE表示白子
    n = len(table)

    pattern1 = str(BLACK) + '{5}'
    pattern2 = str(WHITE) + '{5}'

    # 检查所有横行
    for i in range(n):
        # 将table每一行变成字符串
        to_string = ''.join([str(_) for _ in table[i]])
        # 正则匹配
        search1 = re.search(pattern1, to_string)
        search2 = re.search(pattern2, to_string)
        if search1:
            # 返回第i行,黑棋开始和结束的位置
            return i, search1.start(), i, search1.end() - 1, BLACK
        if search2:
            # 返回第i行,白棋开始和结束的位置
            return i, search2.start(), i, search2.end() - 1, WHITE

    # 检查所有竖列
    for j in range(n):
        # 将table每一列变成字符串
        to_string = ''.join([str(table[i][j]) for i in range(n)])
        # 正则匹配
        search1 = re.search(pattern1, to_string)
        search2 = re.search(pattern2, to_string)
        if search1:
            # 黑棋开始和结束的位置，返回第j列
            return search1.start(), j, search1.end() - 1, j, BLACK
        if search2:
            # 白棋开始和结束的位置，返回第j列
            return search2.start(), j, search2.end() - 1, j, WHITE

    # 检查右上三角
    for k in range(n - 4):
        # 将斜线变成字符串
        to_string = ''.join([str(table[h][k + h]) for h in range(n - k)])
        # 正则匹配
        search1 = re.search(pattern1, to_string)
        search2 = re.search(pattern2, to_string)
        if search1:
            # 黑棋的开始位置和结束位置
            return search1.start(), k + search1.start(), search1.end() - 1, k + search1.end() - 1, BLACK
        if search2:
            # 白棋的开始位置和结束位置
            return search2.start(), k + search2.start(), search2.end() - 1, k + search2.end() - 1, WHITE

    # 检查左下三角
    for k in range(1, n - 4):
        # 将斜线变成字符串
        to_string = ''.join([str(table[k + h][h]) for h in range(n - k)])
        # 正则匹配
        search1 = re.search(pattern1, to_string)
        search2 = re.search(pattern2, to_string)
        if search1:
            # 黑棋的开始位置和结束位置
            return k + search1.start(), search1.start(), k + search1.end() - 1, search1.end() - 1, BLACK
        if search2:
            # 白棋的开始位置和结束位置
            return k + search2.start(), search2.start(), k + search2.end() - 1, search2.end() - 1, WHITE

    # 检查左上三角
    for k in range(n - 4):
        # 将斜线变成字符串
        to_string = ''.join([str(table[n - 1 - k - h][h]) for h in range(n - k)])
        # 正则匹配
        search1 = re.search(pattern1, to_string)
        search2 = re.search(pattern2, to_string)
        if search1:
            # 黑棋的开始位置和结束位置
            return n - 1 - k - search1.start(), search1.start(), n - k - search1.end(), search1.end() - 1, BLACK
        if search2:
            # 白棋的开始位置和结束位置
            return n - 1 - k - search2.start(), search2.start(), n - k - search2.end(), search2.end() - 1, WHITE

    # 检查右下三角
    for k in range(1, n - 4):
        # 将斜线变成字符串
        to_string = ''.join([str(table[n - 1 - h][k + h]) for h in range(n - k)])
        # 正则匹配
        search1 = re.search(pattern1, to_string)
        search2 = re.search(pattern2, to_string)
        if search1:
            # 黑棋的开始位置和结束位置
            return n - 1 - search1.start(), k + search1.start(), n - search1.end(), k + search1.end() - 1, BLACK
        if search2:
            # 白棋的开始位置和结束位置
            return n - 1 - search2.start(), k + search2.start(), n - search2.end(), k + search2.end() - 1, WHITE

    return None  # 若未分出胜负，则返回none，否则返回五个连着的相同颜色棋子的首尾坐标
