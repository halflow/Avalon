#!/usr/bin/python3
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

#写入FIFO队列
def bufwrite(message):
    q.put(message)

#Servers类	
class Servers(SRH):  
    def handle(self):  
        print('got connection from ',self.client_address)
        #self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))  
        while True:  
            data = self.request.recv(1024)  
            if data:     
                l1=(self.client_address[0],data)
                bufwrite(l1)
                print("RECV from ", self.client_address) 
            if not q.empty(): 
                l=q.get()			
                if self.client_address[0]==l[0]:
                    self.request.send(l[1])  
                    print(l)
print('server is running....')  
server = socketserver.ThreadingTCPServer(addr,Servers)  
server.serve_forever()
