#include <iostream>
#include "../detail/node.hpp"

#define ensure(cond,msg)  do { std::cout << msg; if ( !(cond) ) { std::cout << " [ERROR]\n\t" << #cond << std::endl; }\
	else { std::cout << " [OK]" << std::endl; } } while (0)

template<
    class Key = std::size_t,
    class T = std::string,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator< std::pair<const Key, T> >
> class dummy_class
{
	public:
		typedef Key key_type;
		typedef T mapped_type;
		typedef std::pair<const Key, T> value_type;
		typedef std::size_t size_type; //Unsigned integral type (usually std::size_t)
		typedef std::ptrdiff_t difference_type;// Signed integer type (usually std::ptrdiff_t)
		typedef Hash hasher;
		typedef KeyEqual key_equal;
		typedef Allocator allocator_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename std::allocator_traits<Allocator>::pointer pointer_type;
		typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer_type;
		//typedef iterator ForwardIterator;
		//typedef const_iterator Constant forward iterator;
		//typedef local_iterator single bucket but not across buckets iterator;
		//typedef const_local_iterator Constant local iterator;
};

int main() {
   std::cout << "------------------------ Starting Test ------------------------" << std::endl;

   using namespace unordered::detail;
   using namespace unordered::detail::utils;

   constexpr size_t hash_step = log(2ul, CACHE_LINE_SIZE/sizeof(std::hash<size_t>::result_type));
   constexpr size_t starting_offset = sizeof(std::hash<size_t>::result_type)-hash_step;

   ensure(
      sizeof(node<dummy_class<>,hash_step, starting_offset>) == CACHE_LINE_SIZE,
      "Check if the size of node class is equal to the cache line"
   );
   ensure(
      ( alignof(node<dummy_class<>,hash_step, starting_offset>) & (CACHE_LINE_SIZE-1) ) == 0,
      "Check if the node class is porperly aligned"
   );

   std::cout << "---------------------------------------------------------------" << std::endl;
}
