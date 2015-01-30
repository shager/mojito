#!/usr/bin/gnuplot

reset

# wxt

#set terminal wxt size 350,262 enhanced font 'Verdana,10' persist
# png
#set terminal pngcairo size 500,400 enhanced font 'Verdana,10'
#set output 'eval_time.png'
# svg
set terminal svg size 500,400 fname 'Verdana, Helvetica, Arial, sans-serif' fsize '13' dashed
set output 'eval_mem.svg'

# color definitions
set border linewidth 1.5

set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 5 ps 0.9 # --- blue (JIT)
set style line 2 lc rgb '#dd181f' lt 2 lw 2 pt 7 ps 0.9 # --- red (BV)
set style line 3 lc rgb '#32cd32' lt 1 lw 2 pt 9 ps 0.9 # --- green (List)

#errorbar linestyle
set style line 4 lc rgb '#0060ad' lt 1 lw 1 pt 5 ps 0.7 # --- blue (JIT)
set style line 5 lc rgb '#dd181f' lt 1 lw 1 pt 7 ps 0.7 # --- red (BV)
set style line 6 lc rgb '#32cd32' lt 1 lw 1 pt 9 ps 0.7 # --- green (List)

set key top left

set nologscale xy
set xrange [1:]
#set yrange [-50:1300]
set autoscale y

set xlabel 'Number of rules' 
set ylabel 'Memory needed for insertion'

set format y "%g MB"
set format x "%gk"
set xtics add ("1k" 1)
set xtics rotate

f(x) = 0.5 * x

plot 'mem_eval.dat'  u ($1/1000.):($2/1000.) with linespoints t 'Bitvector with JIT' ls 1, \
        '' u ($1/1000.):($3/1000.) with linespoints t 'Simple Bitvector' ls 2, \
        f(x) with line t 'List (estimated)' ls 3,\
        '' u ($1/1000.):($2/1000.):($4/1000.) w yerrorbars notitle ls 4, \
        '' u ($1/1000.):($3/1000.):($5/1000.) w yerrorbars notitle ls 5
