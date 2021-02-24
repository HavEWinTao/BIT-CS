"""分数"""
from fractions import Fraction


def reverse_polish_notation(expression):
    """中缀表达式转后缀"""
    oplist = ['*', '+', '/', '-', '(', ')', ' ', '^']
    result = []
    stack = []
    priority = {'^': 4, '*': 3, '/': 3, '+': 2, '-': 2, '(': 1}
    for item in expression:
        if item not in oplist:
            result.append(int(item))
        else:
            if item == '(':
                stack.append(item)
            elif item == ')':
                temp = stack.pop()
                while temp != '(':
                    result.append(temp)
                    temp = stack.pop()
            else:
                while stack and (priority[item] <= priority[stack[len(stack) - 1]]):
                    result.append(stack.pop())
                stack.append(item)
    while stack:
        result.append(stack.pop())
    return result


def solve_integer_expression(expression):
    """根据后缀表达式计算结果"""
    result = []
    for item in expression:
        if isinstance(item, int):
            result.append(item)
        else:
            val1 = result.pop()
            val2 = result.pop()
            if item == '+':
                result.append(Fraction(val1) + Fraction(val2))
            elif item == '-':
                result.append(Fraction(val2) - Fraction(val1))
            elif item == '*':
                result.append(Fraction(val1) * Fraction(val2))
            elif item == '^':
                result.append(Fraction(val2) ** Fraction(val1))
            elif item == '/':
                try:
                    result.append(Fraction(val2) / Fraction(val1))
                except ZeroDivisionError:
                    return "DivZeroError"
    return result.pop()


def solve(expression):
    """计算表达式结果"""
    return solve_integer_expression(reverse_polish_notation(expression))
