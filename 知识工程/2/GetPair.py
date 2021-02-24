import codecs


# 将句子分解为‘单词 B\I\O\n’的形式
def GetPair(path, output):
    In = codecs.open(path, encoding='utf-8')
    Out = codecs.open(output, 'w', 'utf-8')

    for line in In.readlines():
        words = line.split()
        flag = 0
        for word in words:
            if word.startswith("["):
                flag = 1
                word = word[1:]
            elif "]" in word:
                flag = 3
                word = word[0:word.index("]")]
            word = word.split("/")  # 分割命名实体
            if flag == 1:
                flag = 2
                Out.write(word[0] + "  " + "B" + "\n")
            elif flag == 2:
                Out.write(word[0] + "  " + "I" + "\n")
            elif flag == 3:
                flag = 0
                Out.write(word[0] + " " + "I" + "\n")
            else:
                if word[1] == 'nt':
                    tag = 'I'
                else:
                    tag = 'O'
                Out.write(word[0] + "  " + tag + "\n")
