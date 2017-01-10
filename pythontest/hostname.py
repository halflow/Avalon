#!/usr/bin/python3
# 文件名：server.py

# 导入 socket、sys 模块
import socket
import sys

# 创建 socket 对象
serversocket = socket.socket(
            socket.AF_INET, socket.SOCK_STREAM) 

# 获取本地主机名
host = socket.gethostname()    

print("本地地址: %s" % host)

#print("IP: %s" % socket.gethostbyname(host))

print("EXIP: %s" % socket.gethostbyname_ex(host))
