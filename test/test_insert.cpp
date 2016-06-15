#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <atomic>
#include <stdlib.h>     /* exit */
#include <assert.h>     /* assert */
#include "../hash_trie.hpp"

typedef std::chrono::high_resolution_clock hrclock_t;
typedef hrclock_t::time_point ticks_t;

struct atomic_unsigned : std::atomic<unsigned int>
{
    atomic_unsigned() {}
    atomic_unsigned( const atomic_unsigned& ref ) {
        this->store( ref.load() );
    }
};

constexpr double elapsed( const ticks_t& ini, const ticks_t& end ) {
   return std::chrono::duration_cast< std::chrono::duration<double> >( end - ini ).count();
}

void usage( const char argv0[] ) {
   std::cout << argv0 << " number_of_insertions" << std::endl;
   exit( 0 );
}

template <class Container>
void runTest( unsigned long insertions, bool critical ) {
   std::cout << "----------- Starting Benchmark -----------" << std::endl;
   Container map;

   ticks_t t0 = hrclock_t::now();
   /* ----- Perform insertions ----- */
   if( critical ) {
       std::cout << "CRITICAL" << std::endl;
       #pragma omp parallel for shared( map )
       for ( unsigned long i = 0; i < insertions; ++i ) {
           #pragma omp critical ///< Since the values are atomic the critical is only needed if the map is not thread-safe
           map[i]++;
       }
   }
   else {
       std::cout << "NOT CRITICAL" << std::endl;
       #pragma omp parallel for shared( map )
       for ( unsigned long i = 0; i < insertions; ++i ) {
           map[i]++;
       }
   }
   /* --- END Perform insertions --- */
   ticks_t t1 = hrclock_t::now();
   /* ----- Check result ----- */
   if( map.size() != insertions )
       std::cout << "Check failed." << std::endl;
   else std::cout << "Check successful." << std::endl;
   /* --- END Check result --- */

   std::cout << "----------- Benchmark results -----------" << std::endl;
   std::cout << " Time performing insertions:" << std::endl;
   std::cout << "   | " << elapsed( t0, t1 ) << " s" << std::endl;
}

int main( int argc, char* argv[] ) {
   if ( argc < 2 ) {
      usage( argv[0] );
   }
   unsigned long insertions;
   sscanf(argv[1], "%lu", &insertions);
   std::cout << "---------- UNORDERED MAP ----------" << std::endl;
   runTest< std::unordered_map< unsigned long, std::atomic<unsigned int> > >( insertions, true );
   std::cout << "---------- HASH TRIE ----------" << std::endl;
   runTest< unordered::hash_trie< unsigned long, atomic_unsigned > >( insertions, false );
}
