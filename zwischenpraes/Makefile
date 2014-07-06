# Makefile

help:
	@echo \"make target\" baut das zu \"target.tex\" gehörende pdf-Dokument
	@echo \"make clean\" löscht alles, was keine tex-Quelle ist

%: %.tex
	TEXMFHOME="./texmf" pdflatex --interaction=nonstopmode $@.tex
	#TEXMFHOME="./texmf" latex -interaction=nonstopmode $@.tex
	#dvips -o $@.ps $@.dvi                                         
	#ps2pdf $@.ps

clean:
	$(RM) $(RMFLAGS) *.pdf *.ps *.dvi *.log *.aux *.toc *.snm *.out *.nav

