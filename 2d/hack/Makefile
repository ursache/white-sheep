test:	main.cpp Makefile kernels.o
	g++  -I/usr/local/include/vtk-7.0/ -g kernels.o main.cpp -o test -lvtkCommonDataModel-7.0 -lvtkIOXML-7.0 -lvtkCommonCore-7.0

kernels.o: kernels.cpp kernels.h
	$(CXX) -O4 -fstrict-aliasing -march=native -mtune=native -DNDEBUG -c kernels.cpp

clean:
	rm test

.PHONY: clean
