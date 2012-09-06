
#ich musste die pg-lib vorher noch verlinken:
#ln -s /usr/local/lib/libpq.so /usr/lib/libpq.so

all:	
	-rm test.o
	-rm debug.log
	doc++ xcgi.cpp
	make test

test:	test.o	
	c++ -ggdb -lpq -lc_r -o test test.o

test.o:	test.c
	c++ -I/usr/local/include/pgsql -ggdb -c test.c

demos:
	-mkdir cgi-bin
	-rm debug.log

	c++  -ggdb -c test_2.cpp
	c++  -ggdb -lxbkstringutil1 -lxbkio1 -o test_2 test_2.o
	c++  -c test_1.cpp
	c++  -lxbkstringutil1 -lxbkio1 -o test_1.cgi test_1.o
	cp test_* cgi-bin/
	cp test_1.cgi cgi-bin/test_1.cgi.env
	
	
clean:
	#doku entfernen
	rm *.html
	rm *.class	
