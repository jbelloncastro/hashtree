CXX_FLAGS=-std=c++11
OMP_FLAGS=-fopenmp
INCLUDES=

all: test_word_count

test_word_count: test_word_count.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $? -o $@

test_word_count_omp: test_word_count.cpp
	$(CXX) $(CXX_FLAGS) $(OMP_FLAGS) $(INCLUDES) $? -o $@

clean:
	rm -rf test_word_count test_word_count_omp
