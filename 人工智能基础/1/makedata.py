import sys

import numpy as np
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

VACANT = 0  # 空
BLACK = 1  # 黑子
WHITE = 2  # 白子
BOARD_COLOR = QColor(249, 214, 91)  # 棋盘颜色


# 棋盘类
class Board(QWidget):

    def __init__(self, data=None, pred=None, parent=None):
        super(Board, self).__init__(parent)
        self.n = 15  # 15*15
        self.size = 480 // self.n  # 棋盘大小
        self.dia = 28  # 棋子大小
        table = np.random.randint(0, 3, (self.n, self.n)).tolist()  # 棋盘状态
        if data is None:
            self.table = table
        else:
            self.table = data.tolist()
        # 窗口
        self.setWindowTitle("五子棋")  # 标题
        self.setFixedSize(self.size * self.n, self.size * self.n)
        self.pix = QPixmap(self.size * self.n, self.size * self.n)  # 画布

        painter = QPainter(self)
        p = QPainter(self.pix)

        p.setPen(BOARD_COLOR)
        p.setBrush(QBrush(BOARD_COLOR))
        p.drawRect(0, 0, self.size * self.n, self.size * self.n)
        p.setPen(Qt.black)
        for i in range(self.n):
            p.drawLine(self.size // 2, self.size * i + self.size // 2,
                       self.size * self.n - self.size // 2, self.size * i + self.size // 2)
            p.drawLine(self.size * i + self.size // 2, self.size // 2,
                       self.size * i + self.size // 2, self.size * self.n - self.size // 2)

        for i in range(self.n):
            for j in range(self.n):
                if self.table[i][j] == VACANT:
                    continue
                color = Qt.black if self.table[i][j] == BLACK else Qt.white
                p.setPen(color)
                p.setBrush(QBrush(color))
                p.drawEllipse(j * self.size + (self.size - self.dia) // 2,
                              i * self.size + (self.size - self.dia) // 2,
                              self.dia, self.dia)

        if pred is not None:
            for i in range(self.n):
                for j in range(self.n):
                    if pred[i][j] == VACANT:
                        continue
                    color = Qt.blue
                    p.setPen(color)
                    p.setBrush(QBrush(color))
                    x = j * self.size + (self.size - self.dia) // 2
                    y = i * self.size + (self.size - self.dia) // 2
                    p.drawLine(x, y, x + self.dia, y)
                    p.drawLine(x, y, x, y + self.dia)
                    p.drawLine(x + self.dia, y + self.dia, x, y + self.dia)
                    p.drawLine(x + self.dia, y + self.dia, x + self.dia, y)

        painter.drawPixmap(0, 0, self.pix)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    # 通过Qt绘制数据集，采用随机生成的方式
    for step in range(100):
        board = Board()
        # 保存图片和棋盘状态
        path = "./data/train1/"
        image = path + "image/" + str(step) + ".png"
        board.pix.save(image)
        mat = np.array(board.table)
        location = path + "table/" + str(step)
        np.save(location, mat)
    app.exec_()
