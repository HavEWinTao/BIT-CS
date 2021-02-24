"""生成问题和测试"""
from get_problem import generate_problem
from quiz import quiz


def main():
    """主函数"""
    print("1.generate problem(默认)")
    print("2.quiz")
    print("3.exit")
    while True:
        is_int = True
        mode = input("请输入generate problem or solve problem or quiz or exit： ")
        # 生成问题
        if mode == "1":
            print("1.int(默认)")
            print("2.fraction")
            while True:
                problem = input("请输入int or fraction： ")
                if problem == "1":
                    break
                elif problem == "2":
                    is_int = False
                    break
                else:
                    print("error: input should be int or fraction")
                    continue
            while True:
                num = input("请输入生成题目的数量： ")
                if num.isdigit():
                    num = int(num)
                    break
                else:
                    print("error: input should be a int")
            generate_problem(num=num, is_int=is_int)
        # 答题模式
        elif mode == "2":
            modes = ["1", "2"]
            while True:
                show_style = input("请输入乘方运算符显示风格 1.**  2.^：")
                if show_style in modes:
                    break
                else:
                    print("errot: input should be ** or ^")
            show_style = int(show_style)
            while True:
                fraction = input("请输入测试类型 1.整数(默认)  2.分数：")
                if fraction in modes:
                    break
                else:
                    print("error: input should be 整数 or 分数")
            if int(fraction) == 1:
                is_int = True
            else:
                is_int = False
            quiz(show_style=show_style, is_int=is_int)
        # 退出
        elif mode == "3":
            break
        # 错误输入
        else:
            print("error: input should be generate problem or solve problem or quiz or exit")
            continue


if __name__ == "__main__":
    main()
