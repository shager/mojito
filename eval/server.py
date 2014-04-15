import socket
import atexit
import sys

@atexit.register
def save():
    global count
    print count

if len(sys.argv) > 1:
    PORT = int(sys.argv[1])
else:
    PORT = 65000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", PORT))

buf = bytearray(4096)
count = 0
while 1:
    num_bytes = sock.recv_into(buf, 4096)
    count += num_bytes
