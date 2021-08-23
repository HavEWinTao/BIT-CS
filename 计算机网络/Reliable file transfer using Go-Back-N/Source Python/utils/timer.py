import time


# 计时器
class Timer(object):

    TIMER_STOP = -1

    def __init__(self, duration):
        self.start_time = self.TIMER_STOP
        self.duration = duration

    # 开始计时
    def start(self):
        if self.start_time == self.TIMER_STOP:
            self.start_time = time.time()

    # 结束计时
    def stop(self):
        if self.start_time != self.TIMER_STOP:
            self.start_time = self.TIMER_STOP

    # 判断计时器是否运行
    def running(self):
        return self.start_time != self.TIMER_STOP

    # 判断计时器计时是否结束
    def timeout(self):
        if not self.running():
            return False
        else:
            return time.time() - self.start_time >= self.duration
