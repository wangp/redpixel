all: manual manual.txt manual.htm

manual: manual.sgml
	sgml2txt -f $< 
	mv manual.txt manual

manual.txt: manual.sgml	
	sgml2txt $<

manual.htm: manual.sgml
	sgml2html --dosnames --imagebuttons $<

manual.info: manual.sgml
	sgml2info $< 

clean:
	rm -f manual manual.txt 
	rm -f manual*.htm manual.inf
