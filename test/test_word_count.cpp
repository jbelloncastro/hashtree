#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <atomic>
#include <stdlib.h>     /* exit */
#include <assert.h>     /* assert */

typedef std::chrono::high_resolution_clock hrclock_t;
typedef hrclock_t::time_point ticks_t;

constexpr double elapsed( const ticks_t& ini, const ticks_t& end ) {
   return std::chrono::duration_cast< std::chrono::duration<double> >( end - ini ).count();
}

void usage( const char argv0[] ) {
   std::cout << argv0 << " <input file>" << std::endl;
   exit( 0 );
}

template <class Container>
void runTest( char* inputFile ) {
   std::cout << "----------- Starting Benchmark -----------" << std::endl;
   std::vector<std::string> lines;
   Container map;
   unsigned int rawLines = 0;
   unsigned int rawWords = 0;

   ticks_t t0 = hrclock_t::now();

   /* ----- Read the file and map it into a vector to allow parallel processing ----- */
   std::ifstream inStream( inputFile );
   assert( inStream.is_open() );
   std::string line;
   while ( std::getline( inStream, line ) ) {
      if ( !line.empty() ) {
         lines.push_back( line );
      }
      ++rawLines;
   }
   /* --- END Read the file and map it into a vector to allow parallel processing --- */

   ticks_t t1 = hrclock_t::now();
   std::cout << " File " << inputFile << " has " << rawLines << " lines and ";
   std::cout << lines.size() << " no blank lines." << std::endl;
   ticks_t t2 = hrclock_t::now();

   /* ----- Process the lines ----- */
   #pragma omp parallel for shared( map ) reduction( + : rawWords )
   for ( unsigned int idx = 0; idx < lines.size(); ++idx ) {
      // Ignore comment lines
      if ( lines[idx][0] == '#' || lines[idx][0] == '*' ) continue;

      std::stringstream wordsStream;
      wordsStream << lines[idx];
      for ( std::string word; wordsStream >> word; ) {
         #pragma omp critical ///< Since the values are atomic the critical is only needed if the map is not thread-safe
         map[word]++;
         ++rawWords;
      }
   }
   /* --- END Process the lines --- */

   ticks_t t3 = hrclock_t::now();
   std::cout << " File " << inputFile << " has " << map.size() << " words." << std::endl;
   ticks_t t4 = hrclock_t::now();

   /* ----- Get the 3 more frequent words ----- */
   std::pair<std::string, unsigned int> moreFreq[] = {
      {"This should not be the most frequent word :(", 0},
      {"This should not be the most frequent word :(", 0},
      {"This should not be the most frequent word :(", 0}
   };
   for ( auto it = map.begin(); it != map.end(); ++it ) {
      if ( it->second > moreFreq[0].second ) {
         moreFreq[2] = moreFreq[1];
         moreFreq[1] = moreFreq[0];
         moreFreq[0] = *it;
      } else if ( it->second > moreFreq[1].second ) {
         moreFreq[2] = moreFreq[1];
         moreFreq[1] = *it;
      } else if ( it->second > moreFreq[2].second ) {
         moreFreq[2] = *it;
      }
   }
   /* --- END Get the 3 more frequent words --- */

   ticks_t t5 = hrclock_t::now();
   std::cout << " More frequent words in file are:" << std::endl;
   std::cout << "   | " << moreFreq[0].first << " (" << moreFreq[0].second << " times)" << std::endl;
   std::cout << "   | " << moreFreq[1].first << " (" << moreFreq[1].second << " times)" << std::endl;
   std::cout << "   | " << moreFreq[2].first << " (" << moreFreq[2].second << " times)" << std::endl;

   std::cout << "----------- Benchmark results -----------" << std::endl;
   std::cout << " Input file parsing:" << std::endl;
   std::cout << "   | " << elapsed( t0, t1 ) << " s" << std::endl;
   std::cout << "   | " << ( ( double )( rawLines ) / elapsed( t0, t1 ) ) << " lines/s" << std::endl;
   std::cout << " Word count:" << std::endl;
   std::cout << "   | " << elapsed( t2, t3 ) << " s" << std::endl;
   std::cout << "   | " << ( ( double )( lines.size() ) / elapsed( t2, t3 ) ) << " lines/s" << std::endl;
   std::cout << "   | " << ( ( double )( rawWords ) / elapsed( t2, t3 ) ) << " words/s" << std::endl;
   std::cout << " Get most frequent:" << std::endl;
   std::cout << "   | " << elapsed( t4, t5 ) << " s" << std::endl;
   std::cout << "   | " << ( ( double )( map.size() ) / elapsed( t4, t5 ) ) << " words/s" << std::endl;
   std::cout << "-----------------------------------------" << std::endl;
}

int main( int argc, char* argv[] ) {
   if ( argc < 2 ) {
      usage( argv[0] );
   }
   runTest< std::unordered_map< std::string, std::atomic<unsigned int> > >( argv[1] );
}
