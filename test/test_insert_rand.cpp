#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <atomic>
#include <stdlib.h>     /* exit */
#include <assert.h>     /* assert */
#include <random>
#include "../hash_trie.hpp"

uint64_t* values;

struct atomic_unsigned : std::atomic<unsigned int> 
{
    atomic_unsigned() { this->store(0); }
    atomic_unsigned( const atomic_unsigned& ref ) { this->store( ref.load() ); }
};

typedef std::unordered_map< uint64_t, atomic_unsigned > UnorderedMap;
typedef unordered::hash_trie< uint64_t, atomic_unsigned > HashTrie;
typedef std::chrono::high_resolution_clock hrclock_t;
typedef hrclock_t::time_point ticks_t;

constexpr double elapsed( const ticks_t& ini, const ticks_t& end ) {
   return std::chrono::duration_cast< std::chrono::duration<double> >( end - ini ).count();
}

void usage( const char argv0[] ) {
   std::cout << argv0 << " number_of_insertions range_size" << std::endl;
   exit( 0 );
}

void setUp( size_t insertions, size_t range_size ) {
   std::default_random_engine generator;
   // Param 1: mean, Param 2: standard deviation
   std::uniform_int_distribution<uint64_t> distribution(0, range_size);
   values = (uint64_t*)malloc( insertions * sizeof(uint64_t) );

   #pragma omp parallel for shared(values)
   for( size_t i = 0; i < insertions; ++i ) {
      values[i] = distribution(generator);
   }
}

void tearDown() {
   free( values );
}

template <class Container>
void runTest( size_t insertions, bool critical ) {
   std::cout << "----------- Starting Benchmark -----------" << std::endl;
   Container map;

   ticks_t t0 = hrclock_t::now();
   /* ----- Perform insertions ----- */
   if(critical) {
       std::cout << "CRITICAL" << std::endl;
       #pragma omp parallel for shared( map )
       for ( size_t i = 0; i < insertions; ++i ) {
           uint64_t index = values[i];
           #pragma omp critical ///< Since the values are atomic the critical is only needed if the map is not thread-safe
           map[index]++;
       }
   }
   else {
       std::cout << "NOT CRITICAL" << std::endl;
       #pragma omp parallel for shared( map )
       for ( size_t i = 0; i < insertions; ++i ) {
           uint64_t index = values[i];
           map[index]++;
       }
   }
   /* --- END Perform insertions --- */
   ticks_t t1 = hrclock_t::now();
   /* ----- Check result ----- */
   uint64_t check = 0;
   for ( auto it = map.begin(); it != map.end(); ++it ) {
        check+=it->second; 
   }
   if( check != insertions )
       std::cout << "Check failed. Expecting " << insertions << " but got " << check << "." << std::endl;
   else std::cout << "Check successful." << std::endl;
   /* --- END Check result --- */

   std::cout << "----------- Benchmark results -----------" << std::endl;
   std::cout << " Time performing insertions:" << std::endl;
   std::cout << "   | " << elapsed( t0, t1 ) << " s" << std::endl;
}

int main( int argc, char* argv[] ) {
   if ( argc < 3 ) {
      usage( argv[0] );
   }
   size_t insertions, range_size;
   sscanf(argv[1], "%lu", &insertions);
   sscanf(argv[2], "%lu", &range_size);
   std::cout << "Insertions: " << insertions
             << " Range: " << range_size << std::endl;
   setUp( insertions, range_size );
   std::cout << "---------- UNORDERED MAP ----------" << std::endl;
   runTest<UnorderedMap>( insertions, true );
   std::cout << "---------- HASH TRIE ----------" << std::endl;
   runTest<HashTrie>( insertions, false );
   //std::cout << "---------- HASH TRIE WITH CRITICAL ----------" << std::endl;
   //runTest< unordered::hash_trie< unsigned long, atomic_unsigned > >( insertions, true );
   tearDown();
   return 0;
}
