ispc ?= 1

ifeq "$(ispc)" "1"
	KOBJ = kernels-ispc.o
else
	KOBJ = kernels-cpp.o
endif

test:	main.cpp Makefile $(KOBJ)
	g++  -I/usr/local/include/vtk-7.0/ -g -O3 -std=c++11 $(KOBJ) main.cpp -o test -lvtkCommonDataModel-7.0 -lvtkIOXML-7.0 -lvtkCommonCore-7.0

kernels-cpp.o: kernels.cpp kernels.h Makefile
	$(CXX) -O4 -fstrict-aliasing -march=native -mtune=native -DNDEBUG -c kernels.cpp -o $@

kernels-ispc.o: kernels.ispc kernels.h Makefile
	ispc --target=avx2-i32x8  -O2 kernels.ispc -o $@

clean:
	rm test *.o

.PHONY: clean
