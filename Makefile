PDFLATEX1 = pdflatex -draftmmode -interaction=nonstopmode -synctex=1
PDFLATEX2 = pdflatex -interaction=batchmode -synctex=1
SH 		  = /bin/bash
ASCRIPT   = /usr/bin/osascript

SOURCE   = informeActividadesDesarrolladas.tex
BASE     = "$(basename $(SOURCE))"
PDFFILE  = $(BASE).pdf

default : pdf view 

.PHONY: pdf
pdf: $(SOURCE)
	# run pdflatex and bibtex multiple times to get references right
	$(PDFLATEX1) $(BASE) && biber $(BASE) && $(PDFLATEX1) $(BASE) && $(PDFLATEX2) $(BASE)

.PHONY: view
view: 
	# reload the document in Skim
	#$(SH) skim-view.sh $(BASE)
	open -a Preview.app $(PDFFILE)

.PHONY: git
git:
	git add .
	git commit -m "Project updated by C.Duque as of $$(date +%Y%m%d%H%M%S)"
	git push -u main origin 

.PHONY: clean
clean :
	# remove all TeX-generated files in your local directory
	$(RM) -f -- *.aux *.bak *.bbl *.blg *.log *.out *.toc *.tdo _region.*

