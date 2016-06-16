CXX=g++
CPPFLAGS=
CXXFLAGS=-std=c++11 -O3
OMP_FLAGS=-fopenmp

tests=test_word_count test_insert test_insert_rand

all: sequential openmp

sequential: $(tests)
openmp: $(addsuffix _omp,$(tests))

%_omp: CXXFLAGS+=$(OMP_FLAGS)

%: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

%_omp: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

run: test_word_count input/hamlet.txt input/big.txt
	./test_word_count input/hamlet.txt
	./test_word_count input/big.txt

clean:
	rm -rf test_{word_count,node,insert,insert_rand}{,_omp}
