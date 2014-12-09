#!/bin/sh

# test list
scp ofdatapath-list fpga@192.168.30.101:sdn_paper/ofdatapath
sudo ./test_executor.py -a "list"

# test bitvector
scp ofdatapath-nojit fpga@192.168.30.101:sdn_paper/ofdatapath
sudo ./test_executor.py -a "simple_bv"

# test jit
scp ofdatapath-jit fpga@192.168.30.101:sdn_paper/ofdatapath
sudo ./test_executor.py -a "jit"

