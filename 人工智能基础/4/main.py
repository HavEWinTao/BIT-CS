import sys
from PyQt5.QtWidgets import QApplication
from Gui import GUI
from Player import AIPlayer
from PolicyValue import PolicyValue
from Board import Board

from config import *

if __name__ == "__main__":
    app = QApplication(sys.argv)

    best_policy = PolicyValue(best_model_file)
    mcts_player = AIPlayer(best_policy.policy_value_fn, c_puct=5, playout_num=400)

    board = Board()

    gui = GUI(board=board, mcts=mcts_player, start_player=1)
    gui.show()

    sys.exit(app.exec_())
