#!/usr/bin/Rscript

library(ggplot2)
library(reshape2)

main <- function(argv) {
	input_csv <- argv[1]
	data <- read.csv(input_csv)
	prefix <- strsplit(input_csv, "\\.")[[1]][1]
	output_name <- paste(prefix, ".pdf", sep="")
	d <- melt(data[c("x", "JIT", "Simple_Bitvector", "List")], id=c("x"))
	errors <- melt(data[c("JIT_error", "Simple_Bitvector_error", "List_error")])
	d$e <- errors$value
	plot <- ggplot(d, aes(x=x, y=value, colour=variable)) +
		geom_line(aes(linetype=variable)) +
		geom_point(size=2, aes(shape=variable)) +
		theme_bw() +
		theme(panel.grid.major=element_blank()) +
		theme(panel.grid.minor=element_blank()) +
		geom_errorbar(aes(ymin=value-e, ymax=value+e, linetype=variable), width=0.3)
	ggsave(plot, file=output_name)
}

main(commandArgs(trailingOnly=TRUE))
