#!/usr/bin/python3
#创建SocketServerTCP服务器：  
import socketserver
import queque
from socketserver import StreamRequestHandler as SRH  
from time import ctime  

q=Queuqe.queue(20)
  
host = '192.168.1.108'  
port = 9999  
addr = (host,port)  

#写入buffer
def bufwrite(message):
    q.put(message)
  
class Servers(SRH):  
    def handle(self):  
        print('got connection from ',self.client_address)
        #self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))  
        while True:  
            data = self.request.recv(1024)  
            if not data:   
                break  
            bufwrite(data)  
            print("RECV from ", self.client_address[0]) 
            if not q.empty():        			
                self.request.send(q.get)  
print('server is running....')  
server = socketserver.ThreadingTCPServer(addr,Servers)  
server.serve_forever()