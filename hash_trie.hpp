
#ifndef HASH_TRIE
#define HASH_TRIE

#include "detail/node.hpp"

#include <functional>
#include <memory>

namespace unordered {

template<
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator< std::pair<const Key, T> >
> class hash_trie
{
	public:
		typedef key_type Key;
		typedef mapped_type T;
		typedef value_type std::pair<const Key, T>;
		typedef size_type std::size_t; //Unsigned integral type (usually std::size_t)
		typedef difference_type std::ptrdiff_t;// Signed integer type (usually std::ptrdiff_t)
		typedef hasher Hash;
		typedef key_equal KeyEqual;
		typedef allocator_type Allocator;
		typedef reference value_type&;
		typedef const_reference const value_type&;
		typedef pointer std::allocator_traits<Allocator>::pointer;
		typedef const_pointer std::allocator_traits<Allocator>::const_pointer;
		//typedef iterator ForwardIterator;
		//typedef const_iterator Constant forward iterator;
		//typedef local_iterator single bucket but not across buckets iterator;
		//typedef const_local_iterator Constant local iterator;

	private:
		typedef node<hash_trie> node_type;

		node_type _root;
	public:
		hash_trie() :
			_root()
		{
		}

		operator[]
			
};

} // namespace unordered

#endif // HASH_TRIE
