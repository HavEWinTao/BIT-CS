from utils.CRC_CCITT import CRCCITT

CRC = CRCCITT()

"""
通过python的索引的特点来解决最后一个包长度不足的情况
"""


# 包序号+数据+FCS
def makepack(seq_num, data=b''):
    seq_bytes = seq_num.to_bytes(4, byteorder='little', signed=True)
    FCS = CRC.FCS(data=(seq_bytes + data))
    FCS_bytes = FCS.to_bytes(length=2, byteorder='big')
    return seq_bytes + data + FCS_bytes


# 空包
def make_empty():
    return b''


# 解包，返回包序号+数据+校验结果
def unpack(packet):
    seq_num = int.from_bytes(packet[0:4], byteorder='little', signed=True)
    flag = CRC.Check(packet)
    return seq_num, packet[4:-2], flag
