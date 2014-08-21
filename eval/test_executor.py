#!/usr/bin/env python2

from optparse import OptionParser
import subprocess, time, os, signal

process = 0

def fetch_remote_udp_count():
    tmp_name = "____________SOME_CRAZY_NAME___________________"
    cmd = "../../oldrepo/mininet/util/m h2 netstat -s -u | grep Udp: -A 2 | grep -o -E [0-9]+ | awk '{sum = sum + $1} END {print sum}' > %s" % tmp_name
    os.system(cmd)
    with open(tmp_name, "r") as tmp:
        content = tmp.read()
    os.unlink(tmp_name)
    return int(content)

def execute_on_host(hostname, call_string):
    try:
        #print "../../mininet/util/m " + str(hostname) + " " + str(call_string)
        retval = os.system("../../oldrepo/mininet/util/m " + str(hostname) + " " + str(call_string))
    except subprocess.CalledProcessError:
        print "Error with command " + str(call_string) + " on host " + str(hostname) + "."
        retval = ""
    return retval

# add default entry in routing table of receiver
def set_routing_table():
    execute_on_host("h2", "route add default gw 10.0.0.1 h2-eth0")

def run_sender(tracefile):
    execute_on_host("h1", "timeout 10s /home/samuel/mojito/eval/sender /home/samuel/mojito/eval/" + str(tracefile) + " yes" + " no")

def get_netstat(host):
    netstat_output = execute_on_host(host, "netstat -s -u")
#    print netstat_output
    lines = netstat_output.split("\n")
    for line in lines:
        if line.find("packets to unknown port received") != -1:
            return line.strip().split(" ")[0]
    return 0

def insert_rules_in_switch(ruleset, case):
    ruleset = ruleset[:-12]
    print "Ruleset = " + ruleset
    process = subprocess.Popen("./ruleset_to_trace.py -c" + str(case) + " -i" + ruleset, shell=True)
    process.wait()

def start_mininet():
    global process
    devnull = open('/dev/null', 'w')
    process = subprocess.Popen("mn --topo single,2 --mac --switch user --controller remote", stdout=devnull, stderr=devnull, shell=True, preexec_fn=os.setsid)
    time.sleep(3)

def stop_mininet():
    global process
    os.killpg(process.pid, signal.SIGTERM)
    process.wait()
    devnull = open('/dev/null', 'w')
    process = subprocess.Popen("mn -c", stdout=devnull, stderr=devnull, shell=True)
    process.wait()
    print "Mininet stopped."

def main():
    #parse options
    parser = OptionParser()
    parser.add_option("-t", "--trace-file", dest = "tracefile", default = "",
                      help = "input rule file (relative path from here)", metavar = "FILE")
    parser.add_option("-a", "--algorithm", dest = "algorithm", default = "list", 
                      help = "textual description of algorithm used", metavar = "NAME")

    (options, args) = parser.parse_args()
    tracefile = options.tracefile
    algorithm = options.algorithm

    outfile_name = str(algorithm) + ".eval"
    if os.path.isfile(outfile_name):
        os.remove(outfile_name)
    
    tracefiles = []
    if tracefile == "":
        for filename in os.listdir("rulesets"):
            if filename.endswith("_trace"):
                tracefiles.append(filename)
    else:
        tracefiles.append(tracefile)
    times = []
    packets = []

    for filename in tracefiles:
        case = filename[filename.find("case")-1:filename.find("case")]
        rules = filename[filename.find("2_")+2:filename.find("_", filename.find("2_")+2)]
        rnd_trace = filename[filename.find("_", filename.find("2_")+2)+1:filename.find(".")]
#        for run in range(0, 1):
        while True:
            start_mininet()
            print "Started mininet"
            insert_rules_in_switch("rulesets/" + filename, case)
            set_routing_table()
            
            old_udp_count = fetch_remote_udp_count()
            start = time.time()
            run_sender("rulesets/" + filename)
            duration = time.time() - start
            pkg_count = fetch_remote_udp_count() - old_udp_count
            if pkg_count == 0:
                stop_mininet()
                continue
            print "Time to send trace: " + str(duration)
            times.append(duration)
#            remote_udp = fetch_remote_udp_count()
            print "Packets received: " + str(pkg_count)
            packets.append(pkg_count)
            stop_mininet()
            break
        
        outfile = open(outfile_name, 'a')
        time_string = ""
        for time_ in times:
            time_string += str(time_) + ","
        packet_string = ""
        for packet in packets:
            packet_string += str(packet) + ","
        
        outfile.write(str(case) + "," + str(rules) + "," + str(rnd_trace) + "," + time_string + packet_string + "\n")
        outfile.close()
        times = []
        packets = []

if __name__ == '__main__':
    main()
