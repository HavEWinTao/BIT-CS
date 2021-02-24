import codecs

import GetPair
import Const


def VecPre(path, output):
    In = codecs.open(path, encoding='utf-8')
    Out = codecs.open(output, 'w', 'utf-8')
    GetPair.GetPair(Const.train_set, Const.train_out)
    GetPair.GetPair(Const.validation_set, Const.validation_out)
    GetPair.GetPair(Const.test_set, Const.test_out)
    dictionary = {}
    file = [Const.train_out, Const.validation_out, Const.test_out]
    for filename in file:
        txt = open(filename, encoding='utf-8').readlines()
        for line in txt:
            pair = line.split()
            if pair[0] not in dictionary:
                dictionary[pair[0]] = 1
    for line in In.readlines():
        if line.split()[0] == '-unknown-':
            Out.write(line)
        if line.split()[0] in dictionary:
            Out.write(line)
