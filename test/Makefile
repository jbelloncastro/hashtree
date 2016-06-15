CXX_FLAGS=-std=c++11
OMP_FLAGS=-fopenmp
INCLUDES=

all: test_node test_word_count

test_node: test_node.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $^ -o $@

test_word_count: test_word_count.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $^ -o $@

test_word_count_omp: test_word_count.cpp
	$(CXX) $(CXX_FLAGS) $(OMP_FLAGS) $(INCLUDES) $^ -o $@

test_insert: test_insert.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $^ -o $@

test_insert_omp: test_insert.cpp
	$(CXX) $(CXX_FLAGS) $(OMP_FLAGS) $(INCLUDES) $^ -o $@

test_insert_rand: test_insert_rand.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $^ -o $@

test_insert_rand_omp: test_insert_rand.cpp
	$(CXX) $(CXX_FLAGS) $(OMP_FLAGS) $(INCLUDES) $^ -o $@

run: test_word_count test_node input/hamlet.txt input/big.txt
	./test_node
	./test_word_count input/hamlet.txt
	./test_word_count input/big.txt

clean:
	rm -rf test_word_count test_word_count_omp test_node
