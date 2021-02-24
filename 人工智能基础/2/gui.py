import numpy as np
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

from function import judge, search

VACANT = 0  # 空
BLACK = 1  # 黑子
WHITE = 2  # 白子

BOARD_COLOR = QColor(249, 214, 91)  # 棋盘颜色


# 按钮类
class Button(QToolButton):
    def __init__(self, parent=None):
        super(Button, self).__init__(parent)
        self.setFont(QFont("Microsoft YaHei", 18))
        self.setFixedSize(QSize(100, 40))

    # 鼠标点击事件
    def mousePressEvent(self, event):
        # 鼠标左键
        if event.button() == Qt.LeftButton:
            self.clicked.emit(True)  # 信号
            self.parent().mousePressEvent(event)


# 棋盘类
class Board(QWidget):

    def __init__(self, step=False, black=True, save=False, depth=1, parent=None):
        super(Board, self).__init__(parent)
        self.n = 15  # 15*15
        self.size = 480 // self.n  # 棋盘大小
        self.dia = 28  # 棋子大小
        self.margin = 60  # 下边距

        self.start = False  # 标志棋局是否已经开始
        self.count = 0  # 已下棋子总数
        self.black = black  # 当前棋手,黑子先手
        self.finish = False  # 标志棋局是否已经结束
        self.sequence = []  # 落子历史
        self.table = [[VACANT for j in range(self.n)] for i in range(self.n)]  # 棋盘状态
        self.withdraw_point = None  # 是否悔棋
        self.winner = VACANT  # 获胜方

        self.show_step = step  # 是否展示落子步数
        self.save = save  # 是否保存棋盘状态
        self.depth = depth  # 搜索的深度
        # 窗口
        self.setWindowTitle("五子棋")  # 标题
        self.setFixedSize(self.size * self.n, self.size * self.n + self.margin)
        self.pix = QPixmap(self.size * self.n, self.size * self.n + self.margin)  # 画布
        self.point = None  # 当前落子点

        # 新局按钮
        self.newgame_button = Button(self)
        self.newgame_button.setText("新局")
        self.newgame_button.setStyleSheet("color:white; background-color:blue")
        self.newgame_button.move(self.size * self.n // 3, self.size * self.n)
        self.newgame_button.clicked.connect(self.newgame)  # 执行newgame

        # 悔棋按钮
        self.withdraw_button = Button(self)
        self.withdraw_button.setText("悔棋")
        self.withdraw_button.setStyleSheet("color:white; background-color:gray")
        self.withdraw_button.move(self.size * self.n * 2 // 3, self.size * self.n)
        self.withdraw_button.clicked.connect(self.withdraw)  # 执行withdraw

    # UI绘制
    def paintEvent(self, event):
        painter = QPainter(self)
        p = QPainter(self.pix)

        # 未开始，绘制棋盘
        if not self.start:
            p.setPen(BOARD_COLOR)
            p.setBrush(QBrush(BOARD_COLOR))
            p.drawRect(0, 0, self.size * self.n, self.size * self.n + self.margin)
            p.setPen(Qt.black)
            for i in range(self.n):
                p.drawLine(self.size * i + self.size // 2, self.size // 2,
                           self.size * i + self.size // 2, self.size * self.n - self.size // 2)
                p.drawLine(self.size // 2, self.size * i + self.size // 2,
                           self.size * self.n - self.size // 2, self.size * i + self.size // 2)

        # 若当前轮到白棋，则计算出下一步的落子位置
        if not self.finish and not self.black:
            i, j = search(self.table, self.sequence, self.depth)
            self.point = (i, j)

        # 存在落子点
        if self.point:
            color = Qt.black if self.black else Qt.white
            p.setPen(color)
            p.setBrush(QBrush(color))
            i, j = self.point
            # 判断是否可以落子
            if (i, j) not in self.sequence and 0 <= i < self.n and 0 <= j < self.n:
                self.count += 1
                if not self.start:  # 设为开始
                    self.start = True
                    self.withdraw_button.setStyleSheet("color:white; background-color:blue")
                self.sequence.append((i, j))  # 保存落子记录
                self.table[i][j] = BLACK if self.black else WHITE  # 更改棋盘信息
                # 画矩形内接圆
                p.drawEllipse(j * self.size + (self.size - self.dia) // 2, i * self.size + (self.size - self.dia) // 2,
                              self.dia, self.dia)

                # 是否显示步数
                if self.show_step:
                    color = Qt.white if self.black else Qt.black
                    p.setPen(color)
                    p.setFont(QFont("Bold", 16))
                    p.drawText(j * self.size + (self.size - self.dia) // 2, i * self.size + (self.size - self.dia) // 2,
                               self.dia, self.dia, Qt.AlignCenter, str(self.count))
                # 更换棋手
                self.black = not self.black
            # 判断结果
            result = judge(self.table)
            if result:
                # 对五个子画直线
                p.setPen(Qt.red)
                i1, j1, i2, j2, self.winner = result
                x1 = j1 * self.size + self.size // 2
                y1 = i1 * self.size + self.size // 2
                x2 = j2 * self.size + self.size // 2
                y2 = i2 * self.size + self.size // 2
                p.drawLine(x1, y1, x2, y2)
                self.finish = True  # 棋局结束
            self.point = None

        # 是否悔棋
        if self.withdraw_point:
            p.setBrush(QBrush(BOARD_COLOR))
            for (i, j) in self.withdraw_point:
                # 将已落的子恢复为棋盘颜色
                p.setPen(BOARD_COLOR)
                p.drawEllipse(j * self.size, i * self.size, self.size, self.size)
                p.setPen(Qt.black)
                # 线也需要恢复
                p.drawLine(j * self.size, i * self.size + self.size // 2,
                           (j + 1) * self.size, i * self.size + self.size // 2)
                p.drawLine(j * self.size + self.size // 2, i * self.size,
                           j * self.size + self.size // 2, (i + 1) * self.size)
            self.withdraw_point = None

        # 如果没有结束，左下角棋子的颜色
        if not self.finish:
            color = Qt.black if self.black else Qt.white
        else:
            color = Qt.black if self.winner == BLACK else Qt.white
        p.setPen(color)
        p.setBrush(QBrush(color))
        p.drawEllipse(self.size * self.n // 10 - self.dia * 3 // 4, self.size * self.n,
                      self.dia * 3 // 2, self.dia * 3 // 2)

        # 游戏结束
        if self.finish:
            p.setPen(Qt.red)
            p.setFont(QFont("Microsoft YaHei", 18))
            # 显示哪方获胜
            p.drawText(QRectF(self.size * self.n // 10 - self.dia * 3 // 4, self.size * self.n,
                              self.dia * 3 // 2, self.dia * 3 // 2), Qt.AlignCenter, "胜")

            # 悔棋按钮失效
            self.withdraw_button.setStyleSheet("color:white; background-color:gray")

        painter.drawPixmap(0, 0, self.pix)

        if self.save:
            # 保存图片和棋盘状态
            path = "../1/data/train2/"
            image = path + "image/" + str(self.count) + ".png"
            self.pix.save(image)
            mat = np.array(self.table)
            location = path + "table/" + str(self.count)
            np.save(location, mat)

        # 若之后轮到白棋，则直接update（而不是等待鼠标点击）
        if not self.black:
            self.update()

    # 鼠标点击
    def mouseReleaseEvent(self, event):
        # 左键
        if event.button() == Qt.LeftButton:
            if not self.finish:  # 还没有结束
                # 获取落子坐标
                x, y = event.pos().x(), event.pos().y()
                j, i = x // self.size, y // self.size
                self.point = (i, j)

            self.update()  # 执行paintEvent

    # 新局
    def newgame(self):
        self.start = False
        self.count = 0
        self.black = True
        self.finish = False
        self.sequence = []
        self.table = [[VACANT for j in range(self.n)] for i in range(self.n)]
        self.withdraw_point = None
        # 悔棋按钮变灰
        self.withdraw_button.setStyleSheet("color:white; background-color:gray")

    # 悔棋
    def withdraw(self):
        if self.start and not self.finish:  # 是否可以悔棋
            # 取出两个棋子作为悔棋棋子
            i1, j1 = self.sequence.pop()
            i2, j2 = self.sequence.pop()
            self.withdraw_point = [(i1, j1), (i2, j2)]
            # 设为空子
            self.table[i1][j1] = VACANT
            self.table[i2][j2] = VACANT
            self.count -= 2

            # 如果悔棋后棋盘为空
            if not self.sequence:
                self.start = False
                self.withdraw_button.setStyleSheet("color:white; background-color:gray")
