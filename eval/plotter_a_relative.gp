#!/usr/bin/gnuplot

reset

# wxt

#set terminal wxt size 350,262 enhanced font 'Verdana,10' persist
# png
#set terminal pngcairo size 500,400 enhanced font 'Verdana,10'
#set output 'eval_a.png'
# svg
set terminal svg size 500,400 fname 'Verdana, Helvetica, Arial, sans-serif' fsize '10' dashed
set output 'eval_a_relative.svg'

# color definitions
set border linewidth 1.5

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 5 ps 0.7 # --- blue (JIT)
set style line 2 lc rgb '#dd181f' lt 2 lw 1 pt 7 ps 0.7 # --- red (BV)
set style line 3 lc rgb '#32cd32' lt 3 lw 1 pt 9 ps 0.7 # --- green (List)

set key

set nologscale xy

set xrange [100:]
set yrange [0:35]
#set autoscale y

set xlabel 'Number of rules' 
set ylabel 'Performace factor against List'

set xtics add ("100" 100)
set xtics rotate

plot 'eval.dat_a' u 1:($2/$4) with linespoints t 'Bitvector with JIT' ls 1, '' u 1:($3/$4) with linespoints t 'Simple Bitvector' ls 2\
        #'' u 1:($2):($5) w yerrorbars notitle ls 1, '' u 1:($3/1000):($6/1000) w yerrorbars notitle ls 2, '' u 1:($4/1000):($7/1000) w yerrorbars notitle ls 3
