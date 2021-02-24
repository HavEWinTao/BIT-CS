from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from config import *

BOARD_COLOR = QColor(249, 214, 91)  # 棋盘颜色


class GUI(QWidget):

    def __init__(self, board, mcts, show_step=True, start_player=0, parent=None):
        super(GUI, self).__init__(parent)
        self.n = board_height
        self.size = 480 // self.n  # 棋盘大小
        self.dia = self.size * 9 // 10  # 棋子大小

        board.init_board(start_player)
        self.board = board
        self.AI_player = mcts
        self.show_step = show_step  # 是否展示落子步数
        # 1是玩家,2是电脑
        self.start_player = self.board.players[start_player]
        self.current_player = self.start_player
        self.start = False
        self.end = False
        self.winner = -1
        self.move = -1
        # 窗口
        self.setWindowTitle("五子棋")  # 标题
        self.setFixedSize(self.size * self.n, self.size * self.n)
        self.pix = QPixmap(self.size * self.n, self.size * self.n)  # 画布

    # UI绘制
    def paintEvent(self, event):
        painter = QPainter(self)
        p = QPainter(self.pix)

        # 未开始，绘制棋盘
        if not self.start:
            p.setPen(BOARD_COLOR)
            p.setBrush(QBrush(BOARD_COLOR))
            p.drawRect(0, 0, self.size * self.n, self.size * self.n)
            p.setPen(Qt.black)
            for i in range(self.n):
                p.drawLine(self.size * i + self.size // 2, self.size // 2,
                           self.size * i + self.size // 2, self.size * self.n - self.size // 2)
                p.drawLine(self.size // 2, self.size * i + self.size // 2,
                           self.size * self.n - self.size // 2, self.size * i + self.size // 2)
            self.start = True

        # 若当前轮到电脑，则计算出下一步的落子位置
        if not self.end and self.current_player == 2:
            self.move = self.AI_player.get_action(self.board)
            if self.move != -1 and self.move in self.board.availables:
                print("AI落子位置：", self.board.move_to_location(self.move))
                self.board.do_move(self.move)

        # 判断是否可以落子
        if self.move != -1:
            [i, j] = self.board.move_to_location(self.move)
            i = self.n - 1 - i
            if self.current_player == self.start_player:
                color = Qt.black
            else:
                color = Qt.white
            p.setPen(color)
            p.setBrush(QBrush(color))
            # 画矩形内接圆
            p.drawEllipse(j * self.size + (self.size - self.dia) // 2, i * self.size + (self.size - self.dia) // 2,
                          self.dia, self.dia)
            # 是否显示步数
            if self.show_step:
                color = Qt.white if color == Qt.black else Qt.black
                p.setPen(color)
                p.setFont(QFont("Bold", 16))
                p.drawText(j * self.size + (self.size - self.dia) // 2, i * self.size + (self.size - self.dia) // 2,
                           self.dia, self.dia, Qt.AlignCenter, str(len(self.board.states)))
            self.move = -1
            # 更换棋手
            if self.current_player == 1:
                self.current_player = 2
            else:
                self.current_player = 1

        # 判断结果
        end, winner = self.board.game_end()
        if end:
            self.end = True  # 棋局结束

        # 如果结束
        if self.end:
            color = Qt.red
            p.setPen(color)
            p.setFont(QFont("Microsoft YaHei", 22))
            # 显示哪方获胜
            if self.start_player == 1:
                opp = 2
            else:
                opp = 1
            if self.current_player == self.start_player:
                p.drawText(QRectF(self.size * self.n // 10, self.size * self.n // 10,
                                  self.dia * 3, self.dia * 3), Qt.AlignCenter, "白棋胜")
            elif self.current_player == opp:
                p.drawText(QRectF(self.size * self.n // 10, self.size * self.n // 10,
                                  self.dia * 3, self.dia * 3), Qt.AlignCenter, "黑棋胜")
            else:
                p.drawText(QRectF(self.size * self.n // 10, self.size * self.n // 10,
                                  self.dia * 3, self.dia * 3), Qt.AlignCenter, "平局")
        else:
            if self.current_player == 2:
                self.update()

        painter.drawPixmap(0, 0, self.pix)

    # 鼠标点击
    def mouseReleaseEvent(self, event):
        # 左键
        if event.button() == Qt.LeftButton:
            if not self.end:  # 还没有结束
                # 获取落子坐标
                x, y = event.pos().x(), event.pos().y()
                j, i = x // self.size, y // self.size
                i = self.n - 1 - i
                print("玩家落子位置：", i, j)
                self.move = self.board.location_to_move([i, j])
                if self.move != -1 and self.move in self.board.availables:
                    self.board.do_move(self.move)
                    self.update()  # 执行paintEvent
