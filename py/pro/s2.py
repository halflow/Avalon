#!/usr/bin/python3
#server接收client发来的消息，进入fifo之后取出，并广播给所有的cilent（不包括发来的）
#创建SocketServerTCP服务器：  
import socketserver,socket,queue
from socketserver import StreamRequestHandler as SRH,BaseServer as BS,TCPServer as TCPS,ThreadingMixIn as TMI
from time import ctime  

#建立一个FIFO队列，括号内是max长度
q=queue.Queue(20)
  
host = '192.168.1.108'  
port = 9998 
addr = (host,port)  
#列表保存所有的socket
connection_list=[]

#写入FIFO队列
def bufwrite(message):
    q.put(message)

sk=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#所有的反馈信息都广播到所有的socket
def broadcast(sock,data_sent):
    for socketid in connection_list:
        #反馈信息不发给master socket和发消息来的client socket
        #if socketid!=server_socket and socketid!=sock:
        if socketid!=sock:
            try:
                socketid.send(data_sent)
            except:
                #如果发送错误，则删除这个client socket
                socketid.close()
                connection_list.remove(socketid)

#Servers类	
class Myhandler(SRH):  
    def handle(self):  
        print('got connection from ',self.client_address)
        sockfd=self.request.accept()
        connection_list.append(sockfd)
        #self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))  
        while True:  
            data = self.request.recv(1024)  
            if data:
                l1=(sockfd,data)
                bufwrite(l1)
                #sk.sendto(data,self.client_address)

class ThreadingTCPServer(TMI,TCPS):
    def service_actions(self):
            if not q.empty(): 
                l=q.get()
                broadcast(l[0],l[1])
                #broadcast(l[0],l[1])

print('server is running....')  
s=ThreadingTCPServer(addr,Myhandler)
s.serve_forever()