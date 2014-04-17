#!/usr/bin/gnuplot

reset

#set terminal wxt size 350,262 enhanced font 'Verdana,10' persist
# png
set terminal pngcairo size 700,560 enhanced font 'Verdana,10'
set output 'test_initial.png'
# svg
#set terminal svg size 350,262 fname 'Verdana, Helvetica, Arial, sans-serif' fsize '10'
#set output 'plotting_data3.svg'

# color definitions
set boxwidth 0.6 relative

set style fill solid
set title 'UDP datarate'

set xlabel 'Number of flowtable entries' 
set ylabel 'Maximum throughput (MBit/s)'

plot 'test_initial.dat' using 1:2 with boxes notitle

