all: text html

manual: manual.sgml
	sgml2txt -f $< 
	mv manual.txt manual

manual.txt: manual.sgml	
	sgml2txt $<

manual.htm: manual.sgml
	sgml2html --dosnames $<

manual.info: manual.sgml
	sgml2info $<

manual.dvi: manual.sgml
	sgml2latex $<

check: manual.sgml
	sgmlcheck $<

text: manual manual.txt
html: manual.htm
info: manual.info
dvi: manual.dvi	

clean:
	rm -f manual manual.txt 
	rm -f manual*.htm manual.info
	rm -f manual.dvi
