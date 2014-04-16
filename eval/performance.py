#!/usr/bin/python

import subprocess

# initialize mininet
subprocess.Popen("mn  --topo single,2 --mac --switch user --controller remote&")

# configure dpctl
oldnum = 0
for num_entries in xrange(10, 1000, 10):
    if oldnum != 0:
        subprocess.Popen("for i in {3.." + str(oldnum) + "}; do dpctl unix:/tmp/s1 flow-mod table=0,cmd=del in_port=$i apply:output=1; done")
    subprocess.Popen("for i in {3.." + str(num_entries) + "}; do dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=$i apply:output=1; done")
    subprocess.Popen("dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=2 apply:output=1")
    subprocess.Popen("dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=1 apply:output=2")
    subprocess.Popen("/home/brack/mininet/util/m h1 iperf -us &")
    output = subprocess.Popen("/home/brack/mininet/util/m h2 iperf -c 10.0.0.1 -u -b 500000000 -t 60")
    
    for line in output.stdout:
        if "%" in line:
            print line
    
    oldnum = num_entries
