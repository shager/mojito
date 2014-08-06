#!/usr/bin/env python2

from optparse import OptionParser
import subprocess, time

def execute_on_host(hostname, call_string):
    try:
        retval = subprocess.check_output("../../oldrepo/mininet/util/m " + str(hostname) + " " + str(call_string), shell=True)
    except subprocess.CalledProcessError:
        print "Error with command " + str(call_string) + " on host " + str(hostname) + "."
        retval = ""
    return retval

# add default entry in routing table of receiver
def set_routing_table():
    execute_on_host("h2", "route add default gw 10.0.0.1 h2-eth0")

def run_sender(tracefile):
    execute_on_host("h1", "/home/samuel/studium/ba/mojito/eval/sender /home/samuel/studium/ba/mojito/eval/" + str(tracefile) + " no" + " no")

def get_netstat(host):
    netstat_output = execute_on_host(host, "netstat -s -u")
#    print netstat_output
    lines = netstat_output.split("\n")
    for line in lines:
        if line.find("packets to unknown port received") != -1:
            return line.strip().split(" ")[0]
    return 0

def main():
    #parse options
    parser = OptionParser()
    parser.add_option("-t", "--trace-file", dest = "tracefile", default = "rulesets/2_100_0.random.rules.bcase_trace",
                      help = "input rule file (relative path from here)", metavar = "FILE")

    (options, args) = parser.parse_args()
    tracefile = options.tracefile

    set_routing_table()
    
    old_udp_count = int(get_netstat("h2"))
    start = time.time()
    run_sender(tracefile)
    duration = time.time() - start
    print "Time to send trace: " + str(duration)
    print "Packets received: " + str(int(get_netstat("h2")) - old_udp_count)

if __name__ == '__main__':
    # Tell mininet to print useful information
    main()
