import socket

from utils.config import *


class UDP:
    def __init__(self, addr):
        # ipv4地址+udp
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # 绑定端口
        self.sock.bind(addr)

    # 发送一个包
    def send(self, packet, addr):
        self.sock.sendto(packet, addr)
        return

    # 接收一个包
    def receive(self):
        packet, addr = self.sock.recvfrom(4096)
        return packet, addr

    def close(self):
        self.sock.close()
