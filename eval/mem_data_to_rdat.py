#!/usr/bin/env python2

import os, math
from optparse import OptionParser

def mean(values):
    assert len(values) > 0
    return sum(values) / float(len(values))


def standard_deviation(values):
    assert len(values) > 0
    n = float(len(values))
    mean_value = mean(values)
    sum_ = sum([(value - mean_value) ** 2 for value in values])
    factor = 1.0 / (n - 1)
    return math.sqrt(factor * sum_)


def mean_and_95_confidence_interval(values):
    n = len(values)
    mean_value = mean(values)
    std_dev = standard_deviation(values)
    factor = 1.96
    error = factor * (std_dev / math.sqrt(n))
    return mean_value, error

def main():
    inputfiles = ["jit.mem_eval", "simple_bv.mem_eval"]
    
    #sort files
    for filename in inputfiles:
        if not os.path.isfile(filename):
            print "Error: File " + filename + " not found!"
        fdes = open(filename, 'r')
        content = fdes.read()
        fdes.close()
        lines = content.split("\n")
        lines.sort()
        fdes = open(filename, 'w')
        for line in lines:
            if line == "":
                continue
            fdes.write(line + "\n")
            
        fdes.close()
        
    #generate intermediate data with average and std dev
    md_list = []
    for filename in inputfiles:
        fdes = open(filename, 'r')
        content = fdes.read()
        fdes.close()
        lines = content.split("\n")
        mem_dict = {1000:[], 3000:[], 5000:[], 7000:[], 9000:[], 11000:[], 13000:[], 
                        15000:[], 17000:[], 19000:[], 21000:[], 23000:[], 25000:[], 27000:[], 
                        29000:[], 31000:[], 33000:[], 35000:[], 37000:[]}
        for key in range(1000, 37001, 2000):
            for line in lines:
                if line == "":
                    continue
                if line.split(",")[0] == str(key):
                    mem_dict[key].append(line.split(",")[2])
                    
        for value in mem_dict:
            mem_dict[value] = mean_and_95_confidence_interval(map(float, mem_dict[value]))
        md_list.append(mem_dict)

    #generate R data files
    output_string = "#\"x\",\"JIT\",\"Simple_Bitvector\",\"JIT_error\",\"Simple_Bitvector_error\"\n"
    for i in range(1000, 37001, 2000):
        output_string += str(i) + " " #indicate our sample size
        output_string += str(md_list[0][i][0]) + " " #get algorithm 1
        output_string += str(md_list[1][i][0]) + " " #get algorithm 2
        output_string += str(md_list[0][i][1]) + " " #get algorithm 1 error
        output_string += str(md_list[1][i][1]) + " " #get algorithm 2 error
        output_string += "\n"
    print output_string
    fdes = open("mem_eval.dat", 'w')
    fdes.write(output_string)
    fdes.close()
    

if __name__ == '__main__':
    main()
