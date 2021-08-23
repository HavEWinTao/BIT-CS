# Reliable file transfer using Go-Back-N(Python版本)

## 文件说明

* log文件夹
    * server_log : 服务端的日志
    * client_log : 客户端的日志
* receivefile文件夹
    * 接收到的文件

* sendfile文件夹
    * 用来测试的要发送的文件

* utils文件夹
    * config.py : 基本配置信息
    * CRC_CCITT.py : CRC校验码的生成以及验证
    * File.py : 文件处理,将文件处理成包也可以将包生成文件
    * PDU.py : 发送端和接收端对包的处理函数
    * timer.py : 计时器类
    * udp.py : 封装的UDP类,用于发送和接收
    
* server.py : 服务端
* client.py : 客户端

## 使用说明

通过修改utils.config中的Server_ip和Client_ip来配置发送机和接收机的ip地址,运行receiver.py和sender.py即可实现文件传送，实现全双工通信

在Pycharm下运行时需要修改运行配置，加入参数,参数为要发送文件的路径,接收到的文件保存在receivefile文件夹中.
    
命令行运行时:
    
* ```python server.py paragram(send file path)```

* ```python client.py paragram(save file path)```
