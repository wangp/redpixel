all: html

manual.txt: manual.sgml
	sgml2txt -f $< 

manual.html: manual.sgml
	sgml2html $<
	for i in *.html; do								\
		sed -e 's/<BODY>/<BODY BGCOLOR="#FFFFFF" TEXT="#000000">/' $$i |	\
		sed -e 's/^<H2>/<BR><H2>/' > _tmp;					\
		mv _tmp $$i;								\
	done

manual.info: manual.sgml
	sgml2info $<

manual.dvi: manual.sgml
	sgml2latex $<

check: manual.sgml
	sgmlcheck $<

text: manual.txt
html: manual.html
info: manual.info
dvi: manual.dvi	

clean:
	rm -f manual.txt manual*.html manual.info manual.dvi
