#!/usr/bin/env python2

import time, subprocess, sys

def insert(i):
    process = subprocess.Popen("../src/test " + str(i), shell=True)
    process.wait()

def main(argv):
    time_list = []
    for i in range(1000, 42001, 2000):
        for run in range(0, 10):
            begintime = time.time()
            insert(i)
            duration = time.time() - begintime
            print "Inserted " + str(i) + " rules, duration: " + str(duration)
            time_list.append(",".join([str(i), str(run), str(duration)]))
    
    f = open(str(argv[0]) + ".time_eval",'w')
    for line in time_list:
        f.write(line + "\n")
    f.close()

if __name__ == '__main__':
    main(sys.argv[1:])
