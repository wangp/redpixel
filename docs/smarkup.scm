#!/usr/local/bin/scsh -s
-*- Scheme -*-

  A simple markup system using Scheme syntax.  This is really just a
  quick prototype to test out the idea (in particular, the tags
  resembled the HTML tags far too closely).  The advantage of using a
  real programming language is that you can later embed programs into
  document (e.g. to do more complex things, e.g. recursive macros).

  This program should be work with very few changes with other Scheme
  implementations.  The main problem is that we need a case-sensitive
  (or case-preserving) Scheme.

  2001-12-20

!#


(define NL (string #\newline))

(define (stringify obj)
  (cond ((symbol? obj) (symbol->string obj))
	((number? obj) (number->string obj))
	(else obj)))

(define current-section 0)
(define current-subsection 0)
(define current-subsubsection 0)


;;; HTML generation.

(define html-toc '())

(define (html-add-toc link text . bold)
  (if (null? bold)
      (set! bold #f))
  (set! html-toc
	(cons (string-append (if bold "<b>" "")
			     "<a href=\"#" link "\">" text "</a><br>"
			     (if bold "</b>" "") NL)
	      html-toc)))

(define (html-indent-toc)
  (set! html-toc (cons "<blockquote>" html-toc)))

(define (html-unindent-toc)
  (set! html-toc (cons "</blockquote>" html-toc)))

(define (html-document title subtitle . rest)
  (let ((quux (apply handle-expr rest)))
    (string-append
     ;; header
     "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2//EN\">" NL
     "<html><head><title>" title "</title></head>" NL
     "<body bgcolor=\"#FFFFFF\" text=\"#000000\" "
     "link=\"#5050E0\" alink=\"#4040F0\" vlink=\"#6060E0\">" NL
     ;; heading
     "<center><h1>" title "</h1>"
     "<p><em>" subtitle "</em></center>" NL
     ;; toc
     "<br><br><h1>Table of Contents</h1><blockquote>" NL
     (apply string-append (reverse html-toc))
     "</blockquote>" NL
     ;; body
     quux
     ;; footer
     "<br><br></body></html>" NL)))

(define (html-section title . rest)
  (set! current-section (+ current-section 1))
  (set! current-subsection 0)
  (set! current-subsubsection 0)
  (let* ((link (string-append "sec_" (number->string current-section)))
	 (full-title (string-append (number->string current-section) ". "
				    (handle-expr title))))
    (html-add-toc link full-title #t)
    (html-indent-toc)
    (let ((quux (string-append
		 "<a name=\"" link "\"></a>"
		 "<br><hr width=\"90%\"><br><h1>" full-title "</h1>" NL
		 (apply handle-expr rest))))
      (html-unindent-toc)
      quux)))

(define (html-subsection title . rest)
  (set! current-subsection (+ current-subsection 1))
  (set! current-subsubsection 0)
  (let* ((link (string-append "sec_"
			      (number->string current-section) "_"
			      (number->string current-subsection)))
	 (full-title (string-append (number->string current-section) "."
				    (number->string current-subsection) ". "
				    (handle-expr title))))
    (html-add-toc link full-title)
    (string-append "<a name=\"" link "\"></a>"
		   "<br><h2>" full-title "</h2>" NL
		   (apply handle-expr rest))))

(define (html-subsubsection title . rest)
  (set! current-subsubsection (+ current-subsubsection 1))
  (string-append "<br><h3>"
		 (number->string current-section) "."
		 (number->string current-subsection) "."
		 (number->string current-subsubsection) ". "
		 (handle-expr title) "</h3>" NL
		 (apply handle-expr rest)))

(define (html-verbatim . rest)
  (string-append "<pre>" (apply handle-expr rest) "</pre>" NL))

(define (html-list . rest)
  (string-append "<ul>" NL
		 (apply string-append
			(map (lambda (x)
			       (string-append "<li>" (handle-expr x)
					      "</li>")) rest))
		 "</ul>" NL))

(define (html-join . rest)
  (string-append (apply handle-expr rest) NL))

(define (html-link address . rest)
  (string-append "<a href=\"" (stringify address) "\">"
		 (if (null? rest)
		     (stringify address)
		     (apply handle-expr rest))
		 "</a>"))

(define (html-link-target name . rest)
  (string-append "<a name=\"" (stringify name) "\"></a>" NL
		 (apply handle-expr rest)))

(define (html-p . rest)
  (string-append "<blockquote><p>" (apply handle-expr rest)
		 "</blockquote>" NL))

(define (html-bold . rest)
  (string-append "<b>" (apply handle-expr rest) "</b>"))

(define html-backend
  `((document . ,html-document)
    (section . ,html-section)
    (subsection . ,html-subsection)
    (subsubsection . ,html-subsubsection)
    (verbatim . ,html-verbatim)
    (list . ,html-list)
    (join . ,html-join)
    (link . ,html-link)
    (link-target . ,html-link-target)
    (p . ,html-p)
    (bold . ,html-bold)))


;;; Something else.

(define (handle-expr . args)
  (define (handle fst . rest)
    (cond
     ((pair? fst)
      (let ((probe (assoc (car fst) html-backend)))
	(string-append (if probe
			   (apply (cdr probe) (cdr fst))
			   (string-append NL "**UNKNOWN TAG "
					  (stringify (car fst)) "**" NL))
		       (apply handle-expr rest))))
     (else
      (string-append (stringify fst) " "
		     (apply handle-expr rest)))))
  (if (null? args)
      ""
      (apply handle args)))


;;; Entry point.

(define (parse-file file-name)
  (let ((port (open-input-file file-name)))
    (do ((expr (read port) (read port)))
	((eof-object? expr))
      (display (handle-expr expr)))))

(cond ((null? command-line-arguments)
       (display "Need input file.")
       (newline))
      (else
       (parse-file (car command-line-arguments))))
