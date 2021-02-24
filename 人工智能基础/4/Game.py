import numpy as np


class Game(object):

    def __init__(self, board):
        self.board = board  # 用于游戏的棋盘

    # AI模型和其他模型对弈
    # 通过main.py实现人与AI的对弈
    def start_play(self, player1, player2, start_player=0):
        self.board.init_board(start_player)  # 初始化棋盘
        p1, p2 = self.board.players
        player1.set_player_id(p1)
        player2.set_player_id(p2)
        players = {p1: player1, p2: player2}
        # 交替下棋
        while True:
            current_player = self.board.get_current_player()
            player_in_turn = players[current_player]
            # 落子
            move = player_in_turn.get_action(self.board)
            self.board.do_move(move)
            # 判断胜负状态
            end, winner = self.board.game_end()
            if end:
                return winner

    # 开始并执行一局完整的自我对弈
    def start_self_play(self, player, temp=1e-3):
        # temp是温度参数,用来控制自我对弈过程中player.get_action的探索程度
        # player是封装好的实例
        self.board.init_board()  # 棋盘初始化
        # 棋盘状态,落子概率,当前棋手
        states, mcts_probs, current_players = [], [], []
        while True:
            # 当前棋盘下的落子位置和棋盘上每个位置的落子概率
            move, move_probs = player.get_action(self.board, temp=temp, return_prob=1)
            # 保存数据
            states.append(self.board.current_state())
            mcts_probs.append(move_probs)
            current_players.append(self.board.current_player)
            # 确认落子
            self.board.do_move(move)
            end, winner = self.board.game_end()
            if end:  # 判断是否结束
                # 胜负信息
                winners_z = np.zeros(len(current_players))
                if winner != -1:
                    winners_z[np.array(current_players) == winner] = 1.0
                    winners_z[np.array(current_players) != winner] = -1.0
                # 重置棋手
                player.reset_player()
                return winner, zip(states, mcts_probs, winners_z)
