#!/usr/bin/env python2

from optparse import OptionParser
import subprocess, time, os, signal

process = 0

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
    with open('/tmp/s1-ofd.log', 'r') as f:
        first_line = f.readline()
    tmppid = first_line.split("==")[1]
    subprocess.Popen( 'kill -TERM ' + str(tmppid), shell=True )
    time.sleep(1)
    devnull = open('/dev/null', 'w')
    process2 = subprocess.Popen("killall ofdatapath", stdout=devnull, stderr=devnull, shell=True)
    process2.wait()
    os.killpg(process.pid, signal.SIGTERM)
    process.wait()

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

    outfile_name = str(algorithm) + ".mem_eval"
    if os.path.isfile(outfile_name):
        os.remove(outfile_name)
    
    tracefiles = []
    if tracefile == "":
        for filename in os.listdir("rulesets"):
            if filename.endswith("_trace"):
                tracefiles.append(filename)
    else:
        tracefiles.append(tracefile)
    mem_data = []
    time_data = []

    for filename in tracefiles:
        case = filename[filename.find("case")-1:filename.find("case")]
        rules = filename[filename.find("2_")+2:filename.find("_", filename.find("2_")+2)]
        rnd_trace = filename[filename.find("_", filename.find("2_")+2)+1:filename.find(".")]

        start_mininet()
        print "Started mininet"
        start = time.time()
        insert_rules_in_switch("rulesets/" + filename, case)
        duration = str(time.time() - start)
        stop_mininet()
        time.sleep(2)
        #memsize = subprocess.check_output("tail -n1 /tmp/s1-ofd.log", shell=True).strip()
        memsize = 0
        with open('/tmp/massif.out', 'r') as content_file:
            mem_file_content = content_file.read()
        mem_file_lines = mem_file_content.split("\n")
        for mem_file_line in mem_file_lines:
            if mem_file_line.startswith("mem_heap_B="):
                cur_size = int(mem_file_line[11:])
                if memsize < cur_size:
                    memsize = cur_size
        devnull = open('/dev/null', 'w')
        process3 = subprocess.Popen("mn -c", stdout=devnull, stderr=devnull, shell=True)
        process3.wait()

        memsize = str(memsize)
        print "memsize = " + memsize
        print "duration = " + duration
        mem_data.append(memsize)
        time_data.append(duration)
        
        outfile = open(outfile_name, 'a')
        mem_string = ""
        time_string = ""
        for memsize in mem_data:
            mem_string += str(memsize) + ","
        for timedat in time_data:
            time_string += str(timedat) + ","
        
        outfile.write(str(case) + "," + str(rules) + "," + str(rnd_trace) + "," + mem_string + time_string + "\n")
        outfile.close()
        mem_data = []
        time_data = []

if __name__ == '__main__':
    main()
