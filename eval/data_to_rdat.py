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
    inputfiles = ["jit.eval", "simple_bv.eval", "list.eval"]
    
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
    pd_list = []
    td_list = []
    for filename in inputfiles:
        fdes = open(filename, 'r')
        content = fdes.read()
        fdes.close()
        lines = content.split("\n")
        for case in ["a", "b", "w"]:
            packet_dict = {100:[], 300:[], 500:[], 700:[], 900:[], 1100:[], 1300:[], 
                            1500:[], 1700:[], 1900:[], 2100:[], 2300:[], 2500:[], 2700:[], 
                            2900:[], 3100:[], 3300:[], 3500:[]}
            time_dict = {100:[], 300:[], 500:[], 700:[], 900:[], 1100:[], 1300:[], 
                            1500:[], 1700:[], 1900:[], 2100:[], 2300:[], 2500:[], 2700:[], 
                            2900:[], 3100:[], 3300:[], 3500:[]}
            for key in range(100, 3501, 200):
                for line in lines:
                    if line == "":
                        continue
                    if line.split(",")[1] == str(key) and line.split(",")[0] == str(case):
                        packet_dict[key].append(line.split(",")[4])
                        time_dict[key].append(line.split(",")[3])
                        
            for value in packet_dict:
                packet_dict[value] = mean_and_95_confidence_interval(map(int, packet_dict[value]))
            for value in time_dict:
                time_dict[value] = mean_and_95_confidence_interval(map(float, time_dict[value]))
            pd_list.append(packet_dict)
            td_list.append(time_dict)

    print pd_list
    #generate R data files
    for case in range(0, 3):
        output_string = "#\"x\",\"JIT\",\"Simple_Bitvector\",\"List\",\"JIT_error\",\"Simple_Bitvector_error\",\"List_error\"\n"
        for i in range(100, 3501, 200):
            output_string += str(i) + " " #indicate our sample size
            output_string += str(pd_list[0 + case][i][0]) + " " #get algorithm 1
            output_string += str(pd_list[3 + case][i][0]) + " " #get algorithm 2
            output_string += str(pd_list[6 + case][i][0]) + " " #get algorithm 3
            output_string += str(pd_list[0 + case][i][1]) + " " #get algorithm 1 error
            output_string += str(pd_list[3 + case][i][1]) + " " #get algorithm 2 error
            output_string += str(pd_list[6 + case][i][1])       #get algorithm 3 error
            output_string += "\n"
        case_s = ""
        if case == 0:
            case_s = "a"
        if case == 1:
            case_s = "b"
        if case == 2:
            case_s = "w"
        fdes = open("eval.dat_" + case_s, 'w')
        fdes.write(output_string)
        fdes.close()
    

if __name__ == '__main__':
    main()
