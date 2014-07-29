#!/usr/bin/python2

from optparse import OptionParser
import random, subprocess, socket, struct

# Input:
#  - number of rules to generate
#  - number of connected hosts (assumption: IP addresses are 10.0.0.x, MAC is 00:00:00:00:00:xx)
#  - target: send all traffic to one host
#  - random seed

def int_to_ip(addr):
    return socket.inet_ntoa(struct.pack("!I", addr))

def ip_to_int(addr):
    return struct.unpack("!I", socket.inet_aton(addr))[0]

class Rule:
    def __init__(self, src_ip, dst_ip, src_port, dst_port, protocol, in_port, actions):
        self.src_ip = src_ip
        self.dst_ip = dst_ip
        self.src_port = src_port
        self.dst_port = dst_port
        self.protocol = protocol
        self.in_port = in_port
        self.actions = actions
    def insert_rule(self):
        call_args = []
        call_args.append("idle_timeout=0")
        call_args.append("ip")
        if self.src_ip != -1:
            call_args.append("nw_src=" + int_to_ip(self.src_ip))
        if self.dst_ip != -1:
            call_args.append("nw_dst=" + int_to_ip(self.dst_ip))
        if self.src_port != -1:
            call_args.append("tp_src=" + str(self.src_port))
        if self.dst_port != -1:
            call_args.append("tp_dst=" + str(self.dst_port))
        if self.in_port != -1:
            call_args.append("in_port=" + str(self.in_port))
        if self.protocol != -1:
            call_args.append("nw_proto=" + str(self.protocol))
        call_args.append("actions=" + str(self.actions))
        subprocess.call("/usr/local/bin/dpctl" + " add-flow" + " tcp:127.0.0.1:6634 " + ",".join(call_args), shell=True)
    def rule_to_file(self, filename):
        f = open(filename, 'a')
        f.write("@" + str(int_to_ip(self.src_ip)) + "/32" + "\t" + str(int_to_ip(self.dst_ip)) + "/32\t" + str(self.src_port) + " : " + str(self.src_port + 1) + "\t" + str(self.dst_port) + " : " + str(self.dst_port + 1) + "\t" + str(hex(int(self.protocol))) + "/0xFF" + "\t" + "0x0800/0xFFFF\n")
        f.close()

nrules = 0
nhosts = 0
random_seed = 0
target = 0
sender = 0

parser = OptionParser()
parser.add_option("-n", "--nrules", dest="nrules", default = 1000,
                  help = "number of rules to generate", metavar = "NUMBER")
parser.add_option("-H", "--hosts", dest="nhosts", default = 5,
                  help = "number of hosts in the network", metavar = "NUMBER")
parser.add_option("-r", "--random-seed", dest="random_seed", default = 0,
                  help = "seed for usage of PRNG", metavar = "RANDOM_SEED")
parser.add_option("-t", "--target", dest="target", default = -1,
                  help = "target for traffic", metavar = "TARGET NUMBER")
parser.add_option("-s", "--sender", dest="sender", default = -1,
                  help = "sender for traffic", metavar = "SENDER NUMBER")

(options, args) = parser.parse_args()
nrules = int(options.nrules)
nhosts = int(options.nhosts)
random_seed = int(options.random_seed)
target = int(options.target)
sender = int(options.sender)

# initialize PRNG
random.seed(random_seed)

#allow ARP
subprocess.call("/usr/local/bin/dpctl" + " add-flow" + " tcp:127.0.0.1:6634 " + "arp,idle_timeout=0,actions=all", shell=True)
subprocess.call("/usr/local/bin/dpctl" + " add-flow" + " tcp:127.0.0.1:6634 " + "nw_proto=2,idle_timeout=0,actions=all", shell=True)

# generate rule set from input
rule_set = [] #empty list

for number in range(0, nrules):
    if sender == -1:
        host = int((random.random() * nhosts) % nhosts) + 1
    else:
        host = int(sender)
    if target == -1:
        remote = int((random.random() * nhosts) % nhosts) + 1
    else:
        remote = int(target)
    src_ip = ip_to_int("10.0.0." + str(host))
    dst_ip = ip_to_int("10.0.0." + str(remote))
    src_port = int(random.random() * 65535)
    dst_port = int(random.random() * 65535)
    in_port = str(host)
    protocol = 17 #udp
    cur_entry = Rule(src_ip, dst_ip, src_port, dst_port, protocol, in_port, "all")
    cur_entry.insert_rule()
    cur_entry.rule_to_file("rules_db")
    rule_set.append(cur_entry)

subprocess.call("../trace_generator/trace_generator 1 0.1 2 rules_db", shell=True)
print "done"
