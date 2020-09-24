all: requirements.pdf

requirements.pdf: requirements.md
	pandoc -f markdown -t latex -V geometry:margin=1in -o requirements.pdf hints.md

clean:
	rm -f *.pdf
