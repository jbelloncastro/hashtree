
#ifndef HASH_TRIE_DETAIL_NODE
#define HASH_TRIE_DETAIL_NODE

#include "bucket.hpp"

namespace unordered {
namespace detail {
	
constexpr size_t power( size_t base, size_t exp )
{
	return exp==0?1: base*power(base,exp-1);
}

template<
    class Types,
    std::size_t hash_step, // number of hash bits processed by each step
    std::size_t hash_offset = sizeof(Types::hasher::result_type)-hash_step // current number of bits to offset the hash. default: start with hash value's MSBs
> class node {
	public:
		typedef Types::hasher hasher;

	private:
		typedef node< Types, hash_step, hash_offset+hash_step > next_node_type;
		typedef std::unique_ptr< next_node_type > next_node_pointer;
		typedef std::array< next_node_pointer, Types::cache_line_size/sizeof(next_node_pointer) > table;

		static_assert( traits::is_multiple_of< sizeof(Types::hasher::result_type), hash_step >::value,
		               "hash_step must be a exact divisor of the size of the hash" );

		table _nextNodes;

		static std::size_t compute_index( hasher::result_type hash_value )
		{
			return (hash_value >> hash_offset) & ((1<<hash_step)-1);
		}

	public:
		node( hasher::result_type hash_value, const key_type& key ) :
			_nextNodes()
		{
			node_pointer& next = _nextNodes[ extract_index( hash_value ) ];
			next.reset( new next_node_type( key ) );
		}

		Types::reference get( hasher::result_type hash_value, const key_type& key )
		{
			node_pointer& next = _nextNodes[ extract_index( hash_value ) ];
			if( !next ) {
				next.reset( new next_node_type( hash_value, key ) );
			}
			return next->get( hash, key );
		}
		
		void* operator new( std::size_t count )
		{
			return aligned_alloc( Types::cache_line_size, count * sizeof(node) );
		}
};

// class specialization for leaf nodes (bucket)
template<
    class Types,
    std::size_t hash_step
> class node<Types, hash_step, 0> {
	public:
		typedef Types::hasher hasher;
		typedef Types::key_type key_type;
		typedef Types::value_type value_type;
		typedef Types::reference reference;

	private:
		static_assert( traits::is_multiple_of< sizeof(Types::hasher::result_type), hash_step >::value,
		               "hash_step must be a exact divisor of the size of the hash" );

		typedef bucket<Types> bucket_type;

		// Contains bucket
		bucket_type _elements;

	public:
		node( hasher::result_type hash_value, const key_type& key ) :
			_elements( key )
		{
		}

		reference get( hasher::result_type hash_value, const key_type& key )
		{
			_elements.get( key );
		}
};

} // namespace detail
} // namespace unordered

#endif // HASH_TRIE_DETAIL_NODE

