import operator
import socket
import sys
import threading
import time

counter = 0
lock = threading.Lock()

# 系统设置全局变量：frequency(定期时间间隔),unreachable(不可达距离或代价),maxValidTime(最大等待时间),node_number(系统节点总数)
frequency = 0
unreachable = 0.0
maxValidTime = 0
node_number = 0

# 本节点路由表,数据结构为二维列表
list_self = []

# 本节点维护邻居节点序列
list_maintainNode = []

# 接收定时器列表
list_timer = []

# 中止标志
target = True


def print_save(rs=True, receive_id=0, l=[]):
    global counter
    counter += 1
    s1 = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + "\n"
    if rs:
        s2 = "## Sent.Source Node= " + str(list_neighbor[0][0]) + "; Sequence Number = " + str(counter) + "\n"
        s = s1 + s2
        for i in list_self:
            s += "DestNode = " + str(i[0]) + "; Distance=" + str(i[1]) + "; Neighbor=" + str(i[2]) + "\n"
    else:
        s2 = "## Received.Source Node= " + str(receive_id) + "; Sequence Number = " + str(counter) + "\n"
        s = s1 + s2
        for i in l:
            s += "DestNode = " + str(i[0]) + "; Distance=" + str(i[1]) + "; Neighbor=" + str(i[2]) + "\n"
    print(s)
    f = open("log.txt", "a")
    f.write(s + "\n")
    f.close()


def neighbor_node_init(str_nf):
    # 读取保存邻居节点初始化文件信息，第一个为本节点信息
    # 形参为节点信息文件名，函数返回值为元组列表，元组形式：(节点序号，节点名，代价，端口号，ip地址)

    f = open(str_nf)
    line = f.readline()  # 调用文件的 readline()方法
    list_nni = []  # 储存转换结果
    while line:
        line = line.strip('\n')  # 删除换行符
        strlist = line.split(' ')  # 在每行中截取数据
        # print(strlist)

        # 将数据从str转换为相应类型
        tup = (int(strlist[0]), strlist[1], float(strlist[2]), int(strlist[3]), strlist[4])
        # 将数据元组加入储存列表
        list_nni.append(tup)
        list_maintainNode.append(int(strlist[0]))

        line = f.readline()

    # print(list)
    f.close()

    return list_nni


def sys_config(str_sc):
    # 读取保存系统设置信息
    # 形参为系统设置信息文件名

    global maxValidTime, unreachable, frequency, node_number
    f = open(str_sc)
    line = f.readline()  # 调用文件的 readline()方法
    while line:
        line = line.strip('\n')  # 删除换行符
        strlist = line.split(':')  # 在每行中截取数据
        # print(strlist)

        # 将数据从str转换为相应类型
        if strlist[0] == 'Frequency':
            frequency = int(strlist[1])
        elif strlist[0] == 'Unreachable':
            unreachable = float(strlist[1])
        elif strlist[0] == 'MaxValidTime':
            maxValidTime = int(strlist[1])
        elif strlist[0] == 'Node_number':
            node_number = int(strlist[1])
        else:
            # 额外系统配置信息
            pass

        line = f.readline()

    f.close()


def init_routing_table():
    # 根据初始化文件初始化路由表
    # 设置初始化文件中的节点，其它节点全部为不可达
    for i in range(node_number):
        tar = False
        for j in list_neighbor:
            if i == j[0]:
                list_routing = [i, j[2], i]
                list_self.append(list_routing)
                tar = True
        if not tar:
            list_routing = [i, float("inf"), list_neighbor[0][0]]
            list_self.append(list_routing)


def send_message():
    # 向每个邻居节点发送本节点信息
    for i in range(len(list_neighbor)):
        if list_neighbor[i][0] != list_neighbor[0][0]:
            # 指定udp发送方式、接受方IP地址和端口号
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            server_address = (list_neighbor[i][4], list_neighbor[i][3])
            client_socket.sendto(str((list_neighbor[0][0], list_self)).encode(), server_address)
            client_socket.close()
            print_save()


def update_local(tup):
    price1 = 0.0
    price2 = 0.0
    t = list_self
    # 找到本节点到接收节点的代价
    for i in range(len(list_self)):
        if list_self[i][0] == tup[0]:
            price1 = list_self[i][1]
    # 根据接收的信息更新本节点信息,i为计算与本节点代价的当前维护节点
    for i in range(len(list_self)):
        # 找到接收节点到目标节点的代价
        for j in range(len(tup[1])):
            if list_self[i][0] == tup[1][j][0]:
                price2 = tup[1][j][1]
        if list_self[i][1] > price1 + price2:
            list_self[i][1] = min(list_self[i][1], price1 + price2)
            list_self[i][2] = tup[0]

    # 7、如果 x 的距离向量存在变化 发送 x 的距离向量给每一个邻居结点。
    if not operator.eq(t, list_self):
        send_message()


def send_thread_function():
    # 启动发送线程
    send_message()
    global timer1
    timer1 = threading.Timer(frequency / 1000, send_thread_function)
    timer1.start()


def overdue_function():
    while True:
        if not target:
            break
        if lock.acquire():
            for i in range(len(list_timer)):
                if time.time() - list_timer[i][1] > maxValidTime / 1000:
                    for j in range(len(list_self)):
                        if list_self[i][0] == list_timer[i][0]:
                            list_self[i][1] = float("inf")
                            list_self[i][2] = list_neighbor[0][0]
                    del list_timer[i]
                    send_message()
        lock.release()


def receive_thread_function():
    global target
    print("开始接收路由信息！")
    # 为每个邻居节点设置一个定时器
    if lock.acquire():
        for i in list_maintainNode:
            if i != list_neighbor[0][0]:
                list_timer.append([i, time.time()])
    lock.release()
    thread_overdue = threading.Thread(target=overdue_function)
    thread_overdue.start()
    # 设置udp接收端信息
    address = (list_neighbor[0][4], list_neighbor[0][3])
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(address)
    print(list_timer)
    while target:
        # 接收邻居节点信息
        data, addr = s.recvfrom(2048)
        # 重置定时器
        t = list(eval(data.decode().replace('inf', 'float("inf")')))
        ist = False
        if lock.acquire():
            for i in range(len(list_timer)):
                if list_timer[i][0] == t[0]:
                    list_timer[i][1] = time.time()
                    ist = True
                    break
            if not ist:
                list_timer.append([t[0], time.time()])
        lock.release()
        # 记录信息
        print_save(False, t[0], t[1])
        # 更新本节点信息
        update_local(t)


if __name__ == '__main__':
    # 命令行参数格式：dvsim  节点id  节点所使用的UDP端口号  节点初始化文件名
    # 初始化
    # 1、读取邻居节点初始化文件
    list_neighbor = neighbor_node_init(sys.argv[3])
    # 2、读取系统配置文件
    sys_config('system_configuration_file.txt')
    # 3、初始化本节点路由表
    init_routing_table()
    print(list_self)
    # 4、开启向相邻节点发送本节点的距离向量的定时器
    print("开始发送路由信息！")
    send_thread_function()
    # 5、创建接收处理邻居节点所发信息的线程
    thread_receive = threading.Thread(target=receive_thread_function)
    thread_receive.start()
    # 循环
    while 1:
        stri = input("输入指令(P暂停 S恢复 K终止)：")
        if stri == 'K':
            print("节点终止运行！")
            break
        elif stri == 'P':
            print("节点暂停运行")
            # 中止发送
            timer1.cancel()
            # 中止接收
            target = False
        elif stri == 'S':
            print("节点恢复运行")
            # 开始发送
            init_routing_table()
            print("开始发送路由信息！")
            send_thread_function()
            # 开始接收
            target = True
            thread_receive = threading.Thread(target=receive_thread_function)
            thread_receive.start()
    sys.exit(0)
