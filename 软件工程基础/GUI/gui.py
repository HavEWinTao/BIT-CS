"""Qt"""
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

from get_problem import generate_problem
from solve import solve


class Button(QToolButton):
    """按钮类"""

    def __init__(self, parent=None):
        super(Button, self).__init__(parent)
        self.setFont(QFont("Microsoft YaHei", 18))
        self.setFixedSize(QSize(160, 40))
        self.setStyleSheet("background-color:#2176FF")
        self.setStyleSheet("color:#33A1FD")

    def mousePressEvent(self, event):
        """鼠标点击事件"""
        if event.button() == Qt.LeftButton:
            self.clicked.emit(True)
            self.parent().mousePressEvent(event)


class Gui(QWidget):
    """图形界面"""

    def __init__(self):
        super(Gui, self).__init__(None)

        self.time = 20
        self.cur_time = 0
        self.timer = QTimer()
        self.timer.timeout.connect(self.time_control)

        self.show_type = True
        self.correct = 0
        self.number = 0
        self.cur = 0
        self.problem_list = []
        self.answer_list = []

        self.setWindowTitle("四则运算测试")
        self.window_pale = QPalette()
        self.window_pale.setBrush(self.backgroundRole(), QBrush(QPixmap("./background.png")))
        self.setPalette(self.window_pale)
        self.setFixedSize(900, 600)
        icon = QIcon()
        icon.addPixmap(QPixmap("./icon.ico"), QIcon.Normal, QIcon.Off)
        self.setWindowIcon(icon)

        # 开始测试
        self.start_button = Button(self)
        self.start_button.setText("开始测试")
        self.start_button.move(50, 50)
        self.start_button.clicked.connect(self.start_quiz)

        # 测试类型
        self.type_label = QLabel(self)
        self.type_label.setText("如果进行分数运算测试请勾选")
        self.type_label.setFont(QFont("Microsoft YaHei", 12))
        self.type_label.setFixedSize(280, 40)
        self.type_label.move(50, 170)

        self.problem_type = QCheckBox(self)
        self.problem_type.setFixedSize(40, 40)
        self.problem_type.move(320, 170)

        # 显示方式
        self.power_label = QLabel(self)
        self.power_label.setFont(QFont("Microsoft YaHei", 12))
        self.power_label.setText("乘方显示方式:")
        self.power_label.setFixedSize(180, 40)
        self.power_label.move(50, 220)

        self.power_1_label = QLabel(self)
        self.power_1_label.setFont(QFont("Microsoft YaHei", 12))
        self.power_1_label.setText("^(默认)")
        self.power_1_label.setFixedSize(80, 40)
        self.power_1_label.move(190, 220)
        self.power_1 = QCheckBox(self)
        self.power_1.setFixedSize(40, 40)
        self.power_1.move(265, 220)

        self.power_2_label = QLabel(self)
        self.power_2_label.setFont(QFont("Microsoft YaHei", 12))
        self.power_2_label.setText("**")
        self.power_2_label.setFixedSize(60, 40)
        self.power_2_label.move(300, 225)
        self.power_2 = QCheckBox(self)
        self.power_2.setFixedSize(40, 40)
        self.power_2.move(325, 220)

        # 题目数量
        self.number_label = QLabel(self)
        self.number_label.setText("题目数量：")
        self.number_label.setFont(QFont("Microsoft YaHei", 12))
        self.number_label.setFixedSize(100, 40)
        self.number_label.move(50, 120)

        self.problem_number = QLineEdit(self)
        self.problem_number.setFixedSize(70, 40)
        self.problem_number.move(150, 120)

        # 显示题目
        self.show_problem = QLabel(self)
        self.show_problem.setWordWrap(True)
        self.show_problem.setFixedSize(500, 80)
        self.show_problem.setFont(QFont("Microsoft YaHei", 14))
        self.show_problem.move(50, 270)

        # 输入答案
        self.get_answer = QLineEdit(self)
        self.get_answer.setFixedSize(120, 40)
        self.get_answer.setFont(QFont("Microsoft YaHei", 12))
        self.get_answer.move(50, 450)
        # 提交答案
        self.submit_button = Button(self)
        self.submit_button.setText("下一题")
        self.submit_button.setFixedSize(100, 40)
        self.submit_button.setFont(QFont("Microsoft YaHei", 12))
        self.submit_button.move(200, 450)
        self.submit_button.clicked.connect(self.submit)  # 点击按钮提交
        # 显示时间
        self.show_time = QLabel(self)
        self.show_time.setFixedSize(120, 40)
        self.show_time.setFont(QFont("Microsoft YaHei", 12))
        self.show_time.move(50, 500)
        # 显示历史记录
        self.record_button = Button(self)
        self.record_button.setText("查看历史记录")
        self.record_button.setFont(QFont("Microsoft YaHei", 15))
        self.record_button.move(550, 25)
        self.record_button.clicked.connect(self.show_history)

        self.table_widget = QTableWidget(self)
        self.table_widget.setGeometry(QRect(30, 20, 300, 500))
        self.table_widget.move(550, 75)
        self.table_widget.setWindowOpacity(0.85)
        self.table_widget.setObjectName("tableWidget")
        self.table_widget.setColumnCount(0)
        self.table_widget.setRowCount(0)

    def start_quiz(self):
        """开始测试"""
        if self.problem_number.text() == "":
            QMessageBox.critical(self, "Error", "请输入题目数量")
            return
        self.number = int(self.problem_number.text())

        if self.problem_type.isChecked():
            generate_problem(self.number, is_int=False)
            path = "problem_fraction.txt"
        else:
            generate_problem(self.number, is_int=True)
            path = "problem_int.txt"

        if self.power_2.isChecked():
            if self.power_1.isChecked():
                QMessageBox.critical(self, "Error", "请正确选择乘方表示方式")
                return
            else:
                self.show_type = False
        else:
            self.show_type = True

        file = open(path, "r", encoding="utf8")
        self.problem_list = []
        for prolem in file.readlines():
            prolem = prolem.strip("\n")
            self.problem_list.append(prolem)
            self.answer_list.append(str(solve(prolem.split(" "))))
        file.close()

        self.timer.start(1000)

    def submit(self):
        """提交答案"""
        answer = self.get_answer.text()
        if answer == self.answer_list[self.cur]:
            print("答案正确")
            self.correct += 1
        self.get_answer.clear()
        self.cur += 1
        self.cur_time = 0

        if self.cur == self.number:
            self.cur = 0
            self.timer.stop()
            self.show_problem.clear()
            QMessageBox.information(self, "提示", "测试已结束", QMessageBox.Yes)
            self.write_history()
            return

    def time_control(self):
        """时间控制"""
        self.show_time.setText("剩余时间: %s" % (self.time - self.cur_time))
        if not self.show_type:
            problem = self.problem_list[self.cur].replace("^", "**")
        else:
            problem = self.problem_list[self.cur]
        self.show_problem.setText("%s = " % problem)
        if self.cur_time == self.time:  # 到时间自动提交
            self.submit()
        self.cur_time += 1  # 当前题目的已用时

    def write_history(self):
        """保存历史数据"""
        file = open("history.txt", "a+", encoding="utf8")
        file.write(str(self.number) + " " + str(self.correct) + "\n")

    def show_history(self):
        """展示历史记录"""
        file = open("history.txt", "r+", encoding="utf8")
        record_list = []
        for line in file.readlines():
            record_list.append(line.strip().split(" "))
        row = len(record_list)
        col = len(record_list[0])
        self.table_widget.setRowCount(row)
        self.table_widget.setColumnCount(col)
        self.table_widget.setHorizontalHeaderLabels(['总题数', '正确数'])
        for i in range(row):
            for j in range(col):
                the_item = QTableWidgetItem(record_list[i][j])
                the_item.setFont(QFont('Times', 15, QFont.Black))
                the_item.setTextAlignment(0x0004 | Qt.AlignVCenter)
                self.table_widget.setItem(i, j, the_item)
