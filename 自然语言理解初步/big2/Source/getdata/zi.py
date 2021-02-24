# coding=utf-8
# Author:fan hongtao
# Date:2020-11-28

import codecs
import json
import requests
from bs4 import BeautifulSoup

# 爬虫请求头
headers = {
    'user-agent':
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36'
}


# 下载汉字
def downloader(url):
    response = requests.get(url)
    # 爬取失败
    if response.status_code != 200:
        print('data get failed!')
        return
    # 解析html的内容
    html = BeautifulSoup(response.content.decode('gbk', errors='ignore'), "lxml")
    a = html.find_all('a', target="_blank")
    prefix = 'http://www.zd9999.com'
    words = [prefix + w.get('href') for w in a]
    res = []
    # 需要单独打开每个网页才能获取汉字的内容
    for j in range(0, len(words)):
        response = requests.get(words[j])
        # 爬取失败
        if response.status_code != 200:
            print('data get failed!')
            continue
        content = response.content.decode('gbk', errors='ignore')
        content = content.replace('<br/>', '\n').replace('<br>', '\n')
        wordhtml = BeautifulSoup(content, "lxml")
        td = wordhtml.find_all('table')[4].find_all('td')
        # 只保存汉字和拼音
        res.append({
            'word': td[1].text.strip(),
            'pinyin': td[8].text.strip(),
        })
    return res


zi = downloader('http://www.zd9999.com/zi/index.htm')
# 共101页数据
for i in range(2, 102):
    link = "http://www.zd9999.com/zi/index_" + str(i) + ".htm"
    zi += downloader(link)
    print("第" + str(i) + "页")
print(len(zi))

# 将爬取的数据保存为json格式
jsObject = json.dumps(zi)
file = codecs.open('../data/zi.json', 'w', encoding='utf-8')
file.write(jsObject)
file.close()
