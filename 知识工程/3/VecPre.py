import codecs


def VecPre(path, output):
    In = codecs.open(path, encoding='utf-8')
    Out = codecs.open(output, 'w', 'utf-8')
    dictionary = {}
    file = ['./data/train_out.txt', './data/validation_out.txt', './data/test_out.txt']
    for filename in file:
        txt = open(filename, encoding='utf-8').readlines()
        for line in txt:
            for words in line.split():
                word = words.split('\\')[0]
                if word not in dictionary:
                    dictionary[word] = 1
    for line in In.readlines():
        if line.split()[0] == '-unknown-':
            Out.write(line)
        if line.split()[0] in dictionary:
            Out.write(line)
