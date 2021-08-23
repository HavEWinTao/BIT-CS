import codecs
import sys
import threading
import time

from utils import PDU
from utils.config import *
from utils.File import File
from utils.timer import Timer
from utils.udp import UDP

# 线程互斥锁
mutex = threading.Lock()
# 计时器
send_timer = Timer(Timeout)
# 已经成功发送的包序号
SeqNo = 0
# 包的数量
total_packets = 0


# 设置窗口大小
def set_window_size(num_packets):
    global SeqNo
    return min(SWSize, num_packets - SeqNo)  # 防止最后溢出


# 发送数据
def send_data():
    global mutex
    global send_timer
    global SeqNo
    global total_packets

    # 生成所有包
    packets = File(filename).process()
    # 包的数量
    total_packets = len(packets)

    # 下面要发的起始包序号
    next_to_send = 0
    # 已经发送并确认了的包序号
    SeqNo = 0
    window_size = set_window_size(total_packets)

    # 创建接收线程
    T_receive_ACK = threading.Thread(target=receive_ACK)
    T_receive_ACK.start()

    while SeqNo < total_packets:
        mutex.acquire()
        # 将这个窗口的所有包发出
        while next_to_send < SeqNo + window_size:
            # 发送了一个包
            sock_sender.send(packets[next_to_send], (Server_ip, Client_data_Port))
            next_to_send += 1
        # 开始计时
        if not send_timer.running():
            send_timer.start()
        # 等待截至时间或接收到ACK
        while send_timer.running() and not send_timer.timeout():
            mutex.release()
            time.sleep(SLEEP_INTERVAL)
            mutex.acquire()
        # 未接收到ACK重发
        if send_timer.timeout():
            send_timer.stop()
            next_to_send = SeqNo
        else:  # 设定窗口大小，防止溢出
            window_size = set_window_size(total_packets)
        mutex.release()
    # 发送一个空包
    sock_sender.send(PDU.make_empty(), (Server_ip, Client_data_Port))


# 接收ACK
def receive_ACK():
    global mutex
    global send_timer
    global SeqNo
    global total_packets

    while True:
        # 接收到ACK
        pkt, _ = sock_sender.receive()
        # 解包查看ACK序号
        ack, _, flag = PDU.unpack(pkt)
        # 校验正确，正确接收到ACK
        if flag:
            if ack >= SeqNo:
                mutex.acquire()
                SeqNo = ack + 1
                send_timer.stop()
                mutex.release()
            print('Got ACK', ack)
            log.write("True " + str(ack) + "ACK " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
        else:
            print('Got ACK', ack, "but FCS flag is false")
            log.write("False " + str(ack) + "ACK " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
        if ack == total_packets-1:
            break


# 接收数据包
def receive_data():
    time_flag = False
    expected_num = 0  # 期待接收的包序号
    packets = []
    # 不断监听
    while True:
        # 接收
        pack, _ = sock_receiver.receive()
        # 开始计时的标志
        if not time_flag:
            time_flag = True
        # 已经接收不到包，接收完毕了
        if not pack:
            break
        # 解包查看内容
        seq_num, data, flag = PDU.unpack(pack)
        # flag为真即校验成功才接收
        if flag:
            # 返回ACK
            if seq_num == expected_num:
                packets.append(pack)
                pack = PDU.makepack(expected_num)
                sock_receiver.send(pack, (Server_ip, Client_ACK_Port))
                expected_num += 1
            else:
                # 发送ACK(expected_num-1)
                pack = PDU.makepack(expected_num - 1)
                sock_receiver.send(pack, (Server_ip, Client_ACK_Port))
            # 日志
            print('Got packet', seq_num)
            log.write("True " + str(seq_num) + "packet " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
        else:
            # 日志
            print('Got packet', seq_num, "but FCS flag is false")
            log.write("Fasle " + str(seq_num) + "packet " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
    File(packets).process()


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('ERROR: Please input filename')
        exit()
    else:
        filename = sys.argv[1]

    log = codecs.open("./log/client_log", "w")

    # ipv4地址+udp
    sock_sender = UDP((Client_ip, Server_ACK_Port))
    sock_receiver = UDP((Client_ip, Server_data_Port))
    time.sleep(2)

    total_start = time.time()

    T_receive_data = threading.Thread(target=receive_data)
    T_receive_data.start()
    send_data()
    # 等待子进程结束
    T_receive_data.join()

    total_end = time.time()

    time.sleep(1)
    sock_sender.close()
    sock_receiver.close()

    log.write("total time: " + str(total_end - total_start) + '\n')
    print("client close " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
    log.close()
