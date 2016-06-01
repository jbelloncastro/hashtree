
#ifndef HASH_TRIE
#define HASH_TRIE

#include "detail/node.hpp"
#include "detail/utils.hpp"

#include <functional>
#include <memory>

namespace unordered {

using namespace detail;
using namespace detail::utils;

template<
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator< std::pair<const Key, T> >
> class hash_trie
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

	private:
		constexpr size_t hash_step = log(2, CACHE_LINE_SIZE/sizeof(Hash::result_type));

		typedef node<
			hash_trie,
			hash_step,
			sizeof(Hash::result_type)-hash_step
		> node_type;

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
