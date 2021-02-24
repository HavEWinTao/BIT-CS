import sys

from PyQt5.QtWidgets import QApplication

from gui import Board

if __name__ == "__main__":
    app = QApplication(sys.argv)
    board = Board(step=True)
    board.show()
    sys.exit(app.exec_())
