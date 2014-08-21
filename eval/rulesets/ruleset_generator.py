#!/usr/bin/env python2

from optparse import OptionParser
import random, os, socket, struct

max_ipv4 = 4294967295
max_port = 65535

def append_to_file(filename, string):
    f = open(filename, "a")
    f.write(string)
    f.close()

def generate_rule(sender, s_mask, target, t_mask, src_port, dst_port, in_port, protocol, actions):
    if in_port == "":
        in_port = "-"
    rule_string = sender + "/" + s_mask + " " + target + "/" + t_mask + " "
    rule_string += src_port + " " + dst_port + " " + in_port + " " + protocol + " " + actions + "\n"
    return rule_string

def int_to_ip(addr):
    return socket.inet_ntoa(struct.pack("!I", addr))

def ip_to_int(addr):
    return struct.unpack("!I", socket.inet_aton(addr))[0]

def main():
    #parse options
    parser = OptionParser()
    parser.add_option("-n", "--nrules", dest = "nrules", default = 50,
                  help = "number of rules to generate", metavar = "NUMBER")
    parser.add_option("-H", "--hosts", dest = "nhosts", default = 5,
                  help = "number of hosts in the network", metavar = "NUMBER")
    parser.add_option("-r", "--random-seed", dest = "random_seed", default = 0,
                    help = "seed for usage of PRNG", metavar = "RANDOM_SEED")
    parser.add_option("-t", "--target", dest = "target", default = -1,
                    help = "target for traffic", metavar = "TARGET NUMBER")
    parser.add_option("-s", "--sender", dest = "sender", default = -1,
                    help = "sender for traffic", metavar = "SENDER NUMBER")
    parser.add_option("-o", "--output-file-postfix", dest = "outfile", default = ".rules",
                    help = "postfix for output filename", metavar = "POSTFIX")

    (options, args) = parser.parse_args()
    nrules = int(options.nrules)
    nhosts = int(options.nhosts)
    random_seed = int(options.random_seed)
    _target = int(options.target)
    _sender = int(options.sender)
    outputfile = str(nhosts) + "_" + str(nrules) + "_" + str(random_seed) + options.outfile

    # delete old file if existant
    if os.path.isfile(outputfile):
        os.remove(outputfile)
    # initialize PRNG
    random.seed(random_seed)

    for number in range(0, nrules):
        if _sender == -1:
            s_mask = random.randint(1, 32)
            sender = random.randint(0, max_ipv4) & ((2**32 - 1) << (32 - s_mask))
        else:
            s_mask = 32
            sender = _sender

        if _target == -1:
            t_mask = random.randint(1, 32)
            target = random.randint(0, max_ipv4) & ((2**32 - 1) << (32 - t_mask))
        else:
            t_mask = 32
            target = _target

        src_port = random.randint(0, max_port)
        dst_port = random.randint(0, max_port)
        protocol = 17 # udp

        in_port_selector = random.randint(0, nhosts)
        if in_port_selector == 0:
            in_port = ""
        else:
            in_port = 1
        
        actions = "all"
        rule_string = generate_rule(int_to_ip(sender), str(s_mask), int_to_ip(target), str(t_mask), str(src_port), str(dst_port), str(in_port), str(protocol), actions)
        append_to_file(outputfile, rule_string)

if  __name__ =='__main__':
    main()
