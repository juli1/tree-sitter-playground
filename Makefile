python-test: python-test.c
	c++  -I$(TSROOT)/lib/include python-test.c $(TSPYTHONROOT)/src/parser.c $(TSPYTHONROOT)/src/scanner.cc $(TSROOT)/libtree-sitter.a -o python-test

clean:
	rm -f python-test *~
