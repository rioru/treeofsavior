# Dependencies
import socket
import sys
import thread
from ClientHandler import ClientHandler

# Configurations
PORT = int(sys.argv[1]);

# ==========================================================
server = socket.socket (socket.AF_INET, socket.SOCK_STREAM)

# Bind socket to local host and port
try:
    server.bind(('127.0.0.1', PORT))
except socket.error as msg:
	print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
	sys.exit()

# Start listening on socket
server.listen (10)
print '[CHANNEL SERVER] Socket now listening on port ' + str(PORT);

# Client handler
while True:
	clientSock, addr = server.accept()
	print 'Connected with ' + addr[0] + ':' + str(addr[1])
	ClientHandler (clientSock);

print "Server disconnected."
server.close()