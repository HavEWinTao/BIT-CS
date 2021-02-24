import codecs

exclude = "●○▲△『』·—－‰’０１２３４５６７８９ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＲＳＴＶＷＸＺ"
exclude += "ｈｅｒｃｅｐｔｉｎ①②③④⑤⑥⑦"
all_file = codecs.open("../data/all.txt", "r", encoding='utf-8')
exclude_file = codecs.open("../data/exclude.txt", "w", encoding='utf-8')
for sentence in all_file.readlines():
    flag = True
    for item in exclude:
        if item in sentence:
            flag = False
    if flag:
        exclude_file.write(sentence)
exclude_file.close()
all_file.close()
