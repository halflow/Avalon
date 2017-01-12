#!/usr/bin/python3
#server接收client发来的消息，进入fifo之后取出，并广播给所有的cilent（不包括发来的）
#创建SocketServerTCP服务器：  
import socketserver
import queue
from socketserver import StreamRequestHandler as SRH  
from time import ctime  

#建立一个FIFO队列，括号内是max长度
q=queue.Queue(20)
  
host = '192.168.1.108'  
port = 9993 
addr = (host,port)  
#列表保存所有的socket
connection_list=[]

#写入FIFO队列
def bufwrite(message):
    q.put(message)

#所有的反馈信息都广播到所有的socket
def broadcast(sock,data_sent):
    for socket in connection_list:
        #反馈信息不发给master socket和发消息来的client socket
        if socket!=server_socket and socket!=sock:
            try:
            socket.send(data_sent)
            except:
                #如果发送错误，则删除这个client socket
                socket.close()
                connection_list.remove(socketid)

#Servers类	
class Servers(SRH):  
    def handle(self):  
        print('got connection from ',self.client_address)
        connection_list.append(self.client_address)
        #self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))  
        while True:  
            data = self.request.recv(1024)  
            if data:
                l1=(self.client_address,data)
                bufwrite(l1)

print('server is running....')  
server = socketserver.ThreadingTCPServer(addr,Servers)  
server.serve_forever()

if __name__ == "__main__":
    print('进入main了...')
    while 1:
        if not q.empty(): 
            l=q.get()
            broadcast(l[0],l[1])