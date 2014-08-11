#!/usr/bin/env python2

# produces a trace file and instructs dpctl with the commands to insert the rules from
# the specified input file

from optparse import OptionParser
import os, subprocess

def deploy_rule(dpctl_args):
    subprocess.call("/usr/local/bin/dpctl " + dpctl_args, shell=True)

def write_lines_to_casefile(lines, filename):
    # do nothing if file exists
    if os.path.isfile(filename):
        return 
    f = open(filename,'a')
    for line in lines:
        words = line.split(" ")
        f.write("@" + words[0] + "\t" + words[1] + "\t" + words[2] + " : " + str(int(words[2]) + 1) + "\t" + words[3] + " : " + 
                str(int(words[3]) + 1) + "\t" + "0x11/0xFF\t0x0800/0xFFFF\n")
    f.close()

def generate_trace(ruleset, casefile):
    write_lines_to_casefile(ruleset, casefile)
    if not os.path.isfile(casefile + "_trace"):
        subprocess.call("../acceptance_test/trace_generator/trace_generator 1 0.1 1000 " + casefile, shell=True)

def read_file(filename):
    lines = [line.strip() for line in open(filename)]
    return lines

def main():
    #parse options
    parser = OptionParser()
    parser.add_option("-c", "--case", dest = "case", default = "w",
                       help = "select (w)orst, (a)verage or (b)est case test", metavar = "w, a or c")
    parser.add_option("-i", "--input-file", dest = "infile", default = "rulesets/100_100_0.random.rules",
                       help = "input rule file", metavar = "FILE")
    
    (options, args) = parser.parse_args()
    infile = options.infile
    case = options.case
    if not any(x == case for x in ['w', 'a', 'b']):
        case = "w"

    ruleset = read_file(infile)

    deploy_rule("add-flow tcp:127.0.0.1:6634 arp,idle_timeout=0,actions=all")
    deploy_rule("add-flow tcp:127.0.0.1:6634 nw_proto=2,idle_timeout=0,actions=all")

    for rule in ruleset:
        values = rule.split(" ")
        if values[4] == "-": #input port not specified
            deploy_rule("add-flow tcp:127.0.0.1:6634 nw_src=" + values[0]  + ",nw_dst="
                + values[1] + ",tp_src=" + values[2] + ",tp_dst=" + values[3] + 
                ",nw_proto=" + values[5] + ",ip,idle_timeout=0,actions=" + values[6])
        else:
            deploy_rule("add-flow tcp:127.0.0.1:6634 nw_src=" + values[0]  + ",nw_dst="
                + values[1] + ",tp_src=" + values[2] + ",tp_dst=" + values[3] + 
                ",in_port=" + values[4]  + ",nw_proto=" + values[5] + 
                ",ip,idle_timeout=0,actions=" + values[6])

    if case == "b":
        tracefile = infile + ".bcase"
        generate_trace(ruleset[:len(ruleset) / 3], tracefile)
    elif case == "a":
        tracefile = infile + ".acase"
        generate_trace(ruleset[len(ruleset) / 3:2 * (len(ruleset) / 3)], tracefile)
    else: #case == w 
        tracefile = infile + ".wcase"
        generate_trace(ruleset[2 * (len(ruleset) / 3):], tracefile)

if  __name__ =='__main__':
    main()
