
#ifndef HASH_TRIE_DETAIL_NODE
#define HASH_TRIE_DETAIL_NODE


#include "bucket.hpp"
#include "utils.hpp"

namespace unordered {
namespace detail {

using namespace unordered::detail::utils;

template<
    class Types,
    std::size_t hash_step, // number of hash bits processed by each step
    std::size_t hash_offset  // current number of bits to offset the hash. default: start with hash value's MSBs
> class alignas(CACHE_LINE_SIZE) node {
	public:
		typedef typename Types::hasher hasher;

	private:
		typedef typename Types::hasher::result_type hash_value_t;
		typedef typename Types::key_type key_type;

		typedef node<
			Types,
			min(hash_step, hash_offset),
			max(hash_offset-hash_step, 0ul)
		> next_node_type;

		typedef std::unique_ptr< next_node_type > next_node_pointer;

		typedef std::array< next_node_pointer, power(2, hash_step) > table;

		table _nextNodes;

	public:
		node() : _nextNodes() {}

		node( hash_value_t hash_value, const key_type& key ) :
			_nextNodes()
		{
			next_node_pointer& next = _nextNodes[ extract_index( hash_value ) ];
			next.reset( new next_node_type( key ) );
		}

		typename Types::reference get( hash_value_t hash_value, const key_type& key )
		{
			next_node_pointer& next = _nextNodes[ extract_index( hash_value ) ];
			if( !next ) {
				next.reset( new next_node_type( hash_value, key ) );
			}
			return next->get( hash_value, key );
		}

		std::size_t compute_index( hash_value_t hash_value )
		{
			return (hash_value >> hash_offset) & ((1<<hash_step)-1);
		}
};

// class specialization for leaf nodes (bucket)
template<
    class Types,
    std::size_t hash_step
> class alignas(CACHE_LINE_SIZE) node<Types, hash_step, 0> {
	public:
		typedef typename Types::hasher hasher;
		typedef typename Types::key_type key_type;
		typedef typename Types::value_type value_type;
		typedef typename Types::reference reference;

	private:
		typedef typename Types::hasher::result_type hash_value_t;
		typedef bucket<Types> bucket_type;

		// Contains bucket
		bucket_type _elements;

	public:
		node( hash_value_t hash_value, const key_type& key ) :
			_elements( key )
		{
		}

		reference get( hash_value_t hash_value, const key_type& key )
		{
			_elements.get( key );
		}
};

} // namespace detail
} // namespace unordered

#endif // HASH_TRIE_DETAIL_NODE
