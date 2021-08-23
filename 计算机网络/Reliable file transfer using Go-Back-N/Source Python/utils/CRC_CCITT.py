import crcmod


# 循环冗余校验器
class CRCCITT:

    def __init__(self):
        self.crc16_fun = crcmod.predefined.mkCrcFun('crc-ccitt-false')

    # 生成校验码
    def FCS(self, data):
        return self.crc16_fun(data)

    # 检查接收数据是否在传递过程中发生错误
    def Check(self, frame):
        if self.crc16_fun(frame) == 0:
            return True
        else:
            return False
