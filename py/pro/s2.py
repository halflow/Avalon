#!/usr/bin/python3
#server接收client发来的消息，进入fifo之后取出，并广播给所有的cilent（不包括发来的）
#创建SocketServerTCP服务器：  
import socketserver,socket,queue
from socketserver import StreamRequestHandler as SRH,BaseServer as BS,TCPServer as TCPS,ThreadingMixIn as TMI
from time import ctime  

#建立一个FIFO队列，括号内是max长度
q=queue.Queue(20)
  
host = '192.168.1.108'  
port = 9997 
addr = (host,port)  

#列表保存所有的socket
connection_list=[]

#写入FIFO队列
def bufwrite(message):
    q.put(message)

#sk=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
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
        #或者使用print('got connection from ',self.request.getpeername()),
        #注意:socket.getpeername()只有在双方通信正常时才能使用!!
        
        """!!!!!request就是这个客户端的socket！！！
        参考http://blog.csdn.net/songfreeman/article/details/50750680
        通过调用request, client_address = self.get_request(),
        返回self.socket.accept()
        方法get_request()在类TCPServer中存在，并在_handle_request_noblock(self)中调用
        """
        #设置timeout
        #timeout = 5    
        sockfd=self.request
        #禁用 Nagle’s Algorithm,数据马上发送.setsockopt()里面的各种参数是unix系统或Windows系统提供的,应当去内核查询,(包括SOL_SOCKET等)
        sockfd.setsockopt(socket.IPPROTO_TCP,socket.TCP_NODELAY,True)  
        #禁用TCP延迟ACK算法        
        sockfd.setsockopt(socket.IPPROTO_TCP,socket.TCP_QUICKACK,True)
        connection_list.append(sockfd)
        #self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))  
        while True:  
                data = self.request.recv(50)  
                #print(data.decode())
                if data:
                    l1=(sockfd,data)
                    bufwrite(l1)         
                    
class ThreadingTCPServer(TMI,TCPS):
    #修改request队列为10,缺省值是5
    request_queue_size = 10    
    #允许重用地址,即server关闭后会主动释放这个ip和端口
    allow_reuse_address = True

    def service_actions(self):
            if not q.empty(): 
                l=q.get()
                broadcast(l[0],l[1])
                #broadcast(l[0],l[1])

print('server is running....')  
s=ThreadingTCPServer(addr,Myhandler)
s.serve_forever()