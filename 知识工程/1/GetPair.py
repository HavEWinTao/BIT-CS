import codecs

# 将句子分解为‘单词 词性\n’的形式
def GetPair(path, output):
    In = codecs.open(path, encoding='utf-8')
    Out = codecs.open(output, 'w', 'utf-8')

    for line in In.readlines():
        words = line.split()
        flag = 0
        temp_word = ""
        for word in words:  # 提取[中央/n  人民/n  广播/vn  电台/n]nt  样式的词，最长词，如 :中央人民广播电台/n
            tag = ""
            if word.startswith("["):
                flag = 1
                word = word[1:]
            elif "]" in word:
                flag = 2
                tag = word[word.index("]") + 1:]
                word = word[0:word.index("]")]
            temp = word.split("/")  # 分割命名实体
            if flag == 1:
                temp_word = temp_word + temp[0]
            elif flag == 2:
                temp_word = temp_word + temp[0]
                flag = 0
                Out.write(temp_word + "  " + tag + "\n")
                temp_word = ""
            else:
                Out.write(temp[0] + "  " + temp[1] + "\n")
