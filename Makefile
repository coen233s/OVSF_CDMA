all: ovsf

ovsf:	ovsf.o
	g++ ovsf.o -o ovsf

ovsf.o:	ovsf.cpp
	g++ -c ovsf.cpp

clean:
	rm -rf *o *.h~ *.cpp~ ovsf
