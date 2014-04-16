#!/usr/bin/python

import subprocess, os

# initialize mininet
subprocess.Popen(["/usr/local/bin/mn", "--topo", "single,2", "--mac", "--switch", "user", "--controller", "remote"])

# configure dpctl
oldnum = 0
for num_entries in xrange(10, 1000, 10):
    if oldnum != 0:
        for inport in xrange(3, num_entries):
            subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=del", "in_port=" + str(inport), "apply:output=1"], shell=True)
    
    subprocess.Popen("for i in {3.." + str(num_entries) + "}; do /usr/local/bin/dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=$i apply:output=1; done", shell=True)
    subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=add in_port=2", "apply:output=1"])
    subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=add in_port=1", "apply:output=2"])
    subprocess.Popen(["/home/brack/mininet/util/m", "h1", "iperf", "-us"])
    output = subprocess.Popen(["/home/brack/mininet/util/m", "h2", "iperf", "-c", "10.0.0.1", "-u", "-b", "500000000", "-t", "60"], stdout=subprocess.PIPE)
    
    for line in output.stdout:
        if "%" in line:
            print(line)
    
    oldnum = num_entries
