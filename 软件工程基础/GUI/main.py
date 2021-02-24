import sys

from PyQt5.QtWidgets import QApplication

from gui import Gui

if __name__ == "__main__":
    app = QApplication(sys.argv)
    gui = Gui()
    gui.show()
    sys.exit(app.exec_())
