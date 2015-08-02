import SimpleHTTPServer
import SocketServer

server = SocketServer.TCPServer(("", 80), SimpleHTTPServer.SimpleHTTPRequestHandler)
server.serve_forever()