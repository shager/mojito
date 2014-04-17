#!/usr/bin/python

import subprocess, os, time

FNULL = open(os.devnull, 'w')

# initialize mininet
subprocess.Popen(["/usr/local/bin/mn", "--topo", "single,2", "--mac", "--switch", "user", "--controller", "remote"], stdout=FNULL, stderr=subprocess.STDOUT)
time.sleep(2)

# configure dpctl
oldnum = 3
for num_entries in xrange(10, 1000, 10):
    if oldnum != 3:
        subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=del", "in_port=1",  "apply:output=2"], stdout=FNULL, stderr=subprocess.STDOUT)
        subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=del", "in_port=2",  "apply:output=1"], stdout=FNULL, stderr=subprocess.STDOUT)
    
    for inport in xrange(oldnum, num_entries):
        subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=add", "in_port=" + str(inport), "apply:output=1"], stdout=FNULL, stderr=subprocess.STDOUT)
    subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=add", "in_port=2", "apply:output=1"], stdout=FNULL, stderr=subprocess.STDOUT)
    subprocess.Popen(["/usr/local/bin/dpctl", "unix:/tmp/s1", "flow-mod", "table=0,cmd=add", "in_port=1", "apply:output=2"], stdout=FNULL, stderr=subprocess.STDOUT)

    subprocess.Popen(["/home/brack/mininet/util/m", "h1", "iperf", "-us"], stdout=FNULL, stderr=subprocess.STDOUT)
    output = subprocess.Popen(["/home/brack/mininet/util/m", "h2", "iperf", "-c", "10.0.0.1", "-u", "-b", "500000000", "-t", "120"], stdout=subprocess.PIPE)
    
    for line in output.stdout:
        if "%" in line:
            words = line.split(" ")
            for idw in range(0, len(words)):
                if words[idw] == "Mbits/sec":
                    result = str(num_entries) + " " + words[idw - 1] + "\n"
                    print(result)
                    datafile = open("test_initial.dat", 'a')
                    datafile.write(result)
                    datafile.close()
                    break
                
    oldnum = num_entries
