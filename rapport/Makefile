# Tentative de Makefile

########################################################################
# MACROS                                                               #
########################################################################


# Sources
SRC=rapport

########################################################################
# CIBLES                                                               #
########################################################################


# make sans paramètre lance tout
all:
	#make pdf; make bib; make pdf; make pdf
	make pdf; make pdf

pdf:
	@echo "===== PDF ====="
	pdflatex $(SRC).tex

bib:
	@echo "===== BIB ====="
	bibtex $(SRC).aux

clean:
	@echo "===== Cleaning ====="
	rm -rf *.toc *.out *run.xml *.aux *.log *.blg *.bib *.bbl
