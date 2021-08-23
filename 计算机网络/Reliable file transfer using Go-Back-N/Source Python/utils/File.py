import codecs
import os

from utils import PDU
from utils.config import *


class File:
    def __init__(self, pare):
        self.para = pare
        self.seq_num = 0

    def process(self):
        if isinstance(self.para, str):
            packets = self.Gen_packets()
            return packets
        if isinstance(self.para, list):
            self.Gen_file()

    def Gen_packets(self):
        # 长度2位
        filename = os.path.basename(self.para)
        filename = filename.encode()
        filename_len = len(filename)
        len_bytes = filename_len.to_bytes(1, byteorder='little', signed=True)

        file = codecs.open(self.para, 'rb')

        packets = []

        seq_num = 0

        while True:
            if seq_num == 0:
                data = len_bytes + filename + file.read(DataSize - (1 + filename_len + 6))
            else:
                data = file.read(DataSize - 6)
            if not data:
                break
            packets.append(PDU.makepack(seq_num, data))
            seq_num += 1

        file.close()

        return packets

    def Gen_file(self):
        filename_len = int.from_bytes(self.para[0][4:5], byteorder='little', signed=True)
        filename = self.para[0][5:5 + filename_len].decode()
        file = codecs.open("./receivefile/" + filename, 'wb')
        for i in range(len(self.para)):
            seq_num, data, flag = PDU.unpack(self.para[i])
            if i == 0:
                file.write(data[filename_len + 1:])
            else:
                file.write(data)
        file.close()
