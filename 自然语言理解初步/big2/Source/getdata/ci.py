# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

import time
import json
import requests
from bs4 import BeautifulSoup
import codecs


# 爬虫下载单词
def downloader(url):
    res = []
    response = requests.get(url)
    # 爬取失败
    if response.status_code != 200:
        print('data get failed!')
        return len(res)
    # 解析html的数据
    html = BeautifulSoup(response.content.decode('gbk', errors='ignore'), "lxml")
    a = html.find_all('a', target="_blank")
    prefix = 'http://www.zd9999.com'
    links = [prefix + w.get('href') for w in a]
    # 找到想要的信息所在位置
    for j in range(0, len(links)):
        # 需要单独打开某个单词才能获取这个单词的内容
        response = requests.get(links[j])
        content = response.content.decode('gbk', errors='ignore')
        content = content.replace('<br/>', '\n').replace('<br>', '\n')
        wordhtml = BeautifulSoup(content, "lxml")
        td = wordhtml.find_all('table')[5].find_all('td')
        res.append({
            'words': td[0].text.strip()  # 只保存词，其他内容不保存
        })

    return res


# 记录时间
start_time = time.time()
link = 'http://www.zd9999.com/ci/index.htm'
ci = downloader(link)
# 网页的url，共1958页
for i in range(2, 1959):
    link = "http://www.zd9999.com/ci/index_" + str(i) + ".htm"
    ci += downloader(link)
    print("第" + str(i) + "页")
print(len(ci))  # 单词的总个数

end_time = time.time()
print('总共耗时：', end='')
print(end_time - start_time)

# 将爬取的汉字数据保存为json格式
jsObject = json.dumps(ci)
file = codecs.open('../data/ci.json', 'w', encoding='utf-8')
file.write(jsObject)
file.close()
