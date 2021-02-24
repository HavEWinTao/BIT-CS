"""单元测试"""
import unittest

import get_number
import get_operator
import get_problem
import main


class MyTestCase(unittest.TestCase):
    """单元测试"""

    def test_get_number(self):
        """测试数字获取"""
        for _ in range(100):
            self.assertIsInstance(get_number.get_number(is_int=True), str)
            self.assertIsInstance(get_number.get_number(is_int=False), str)
            self.assertIsInstance(get_number.get_exponent(), str)

    def test_get_operator(self):
        """测试运算符获取"""
        for _ in range(1000):
            self.assertIsInstance(get_operator.get_operator(is_int=False), str)
            self.assertIsInstance(get_operator.get_operator(is_int=True), str)

    def test_getprobem(self):
        """测试题目生成"""
        self.assertIsNone(get_problem.generate_problem(1000, is_int=False))
        self.assertIsNone(get_problem.generate_problem(1000, is_int=True))

    def test_main(self):
        """测试主函数"""
        self.assertIsNone(main.main())


if __name__ == '__main__':
    unittest.main()
