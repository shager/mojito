#!/usr/bin/python

# Arguments: A file of data from a measurement
#
# Result: A new file (<old_filename>.mean) with the mean value of each line and the 95 percent conf interval

import sys, math

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

# check for arguments (we need exactly one)
if len(sys.argv) != 2:
    print("No input file given! Exiting...")
    sys.exit(1)

inputfile = str(sys.argv[1])
outputfile = inputfile + ".mean"

fd = open(inputfile, 'r')

result = ""

curline = fd.readline()
while curline != "":
    # values is the entire line except the first value (index)
    values = [float(i) for i in curline.split(' ')[1:]]
    mean_value, error = mean_and_95_confidence_interval(values)
    
    result += curline.split(' ')[0] + " " + str(mean_value) + " " + str(error) + "\n"
    curline = fd.readline()

fd.close()

#write output file
fd = open(outputfile, 'w')
fd.write(result)
fd.close()
print("Done.")

