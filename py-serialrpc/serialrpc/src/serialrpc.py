import msgpackrpc
import sys
from time import time

server = sys.argv[1]
port = int(sys.argv[2])

print(f'server: {server}, port: {port}')

client = msgpackrpc.Client(msgpackrpc.Address(server, port))
result = client.call('register', 5005, ['tty'])


class Server(object):
    def tty(self, msg):
        print(f'[{time()}] Call {msg}')
        print(msg.decode())
        return len(msg)


rpc_server = msgpackrpc.Server(Server())
rpc_server.listen(msgpackrpc.Address("localhost", 5005))
rpc_server.start()
