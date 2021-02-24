epochs = 40  # 训练轮数

size = 15  # 棋盘大小

channel1 = 12  # 卷积层1的输出通道数
channel2 = 24  # 卷积层2的输出通道数

dim1 = 12  # 线性层1的输出通道数

# 保存loss和correct信息,用于绘图
Loss_list = []
Correct_List = []
