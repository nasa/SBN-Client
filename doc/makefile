all: requirements.pdf

requirements.pdf: requirements.md
	pandoc -f markdown -t latex -V geometry:margin=1in --variable urlcolor=blue -o requirements.pdf requirements.md

clean:
	rm -f *.pdf
