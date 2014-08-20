#!/usr/bin/env python2

import subprocess, sys

def insert(i):
    result = subprocess.check_output("/usr/bin/time -f %M ../src/test 2>&1 " + str(i), shell=True)
    return result

def main(argv):
    mem_list = []
    for i in range(1000, 38001, 2000):
        for run in range(0, 10):
            memory = insert(i)
            memory = memory.strip()
            print "Inserted " + str(i) + " rules, memory: " + str(memory)
            mem_list.append(",".join([str(i), str(run), str(memory)]))
    
    f = open(str(argv[0]) + ".mem_eval",'w')
    for line in mem_list:
        f.write(line + "\n")
    f.close()

if __name__ == '__main__':
    main(sys.argv[1:])
