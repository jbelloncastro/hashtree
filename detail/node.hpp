#ifndef HASH_TRIE_DETAIL_NODE
#define HASH_TRIE_DETAIL_NODE

#include<iostream>
#include<memory>
#include<forward_list>

#include "utils.hpp"

namespace unordered {
namespace detail {

using namespace unordered::detail::utils;

template<
    class Types,
    std::size_t hash_step,	//< Number of hash bits processed by each step
    std::size_t hash_offset	//< Current number of bits to offset the hash. default: start with hash value's MSBs
> class alignas(CACHE_LINE_SIZE) node {
	public:
		typedef typename Types::hasher hasher;
		typedef typename Types::iterator iterator;
		typedef typename Types::value_type value_type;
		typedef typename Types::reference reference;
		typedef typename Types::leaf_list leaf_list;
		typedef typename Types::size_type size_type;

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
		node( leaf_list& list ) : _nextNodes() {}

		iterator find( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			next_node_pointer& next = _nextNodes[ compute_index( hash_value ) ];
			return next ? next->find( hash_value, key, list ) : iterator( list );
		}

		reference get( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			next_node_pointer& next = _nextNodes[ compute_index( hash_value ) ];
			if( !next ) {
				next.reset( new next_node_type( list ) );
			}
			return next->get( hash_value, key, list );
		}

		std::size_t compute_index( hash_value_t hash_value )
		{
			return (hash_value >> hash_offset) & ((1<<hash_step)-1);
		}

		std::pair<iterator,bool> insert( const hash_value_t& hash_value, const value_type& value, leaf_list& list )
		{
			next_node_pointer& next = _nextNodes[ compute_index( hash_value ) ];
			if( !next ) {
				next.reset( new next_node_type( list ) );
			}
			return next->insert( hash_value, value, list );
		}

		size_type erase( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			next_node_pointer& next = _nextNodes[ compute_index( hash_value ) ];
			return next ? next->find( hash_value, key, list ) : 0;

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
		typedef typename Types::mapped_type mapped_type;
		typedef typename Types::reference reference;
		typedef typename Types::value_list value_list;
		typedef typename Types::iterator trie_iterator;
		typedef typename Types::leaf_list leaf_list;
		typedef typename Types::size_type size_type;

	private:
		typedef typename Types::hasher::result_type hash_value_t;
		typedef typename Types::leaf_list::iterator leaf_list_iterator;

		// Contains bucket
		value_list		_elements;
		leaf_list_iterator	_self;
	public:
		node( leaf_list& list ) : _elements()
		{
			_self = list.insert_after( list.before_begin(), &_elements );
		}

		trie_iterator find( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			auto it = _elements.begin();
			while ( it != _elements.end() && it->first != key ) ++it;
			return it == _elements.end() ? trie_iterator( list ) : trie_iterator( list, _self, it );
		}

		reference get( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			auto it = _elements.begin();
			while ( it != _elements.end() && it->first != key ) ++it;
			if ( it == _elements.end() ) {
				value_type tmp( key, mapped_type() );
				it = _elements.insert_after( _elements.before_begin(), tmp );	
			}
			return *it;

		}

		std::pair<trie_iterator,bool> insert( const hash_value_t& hash_value, const value_type& value, leaf_list& list )
		{
			auto it = _elements.begin();
			while ( it != _elements.end() && it->first != value.first ) ++it;
			bool inserted = false;
			if ( it == _elements.end() ) {
				it = _elements.insert_after( _elements.before_begin(), value );	
				inserted = true;
			}
			trie_iterator it2 = trie_iterator( list, _self, it );
			return std::make_pair( it2, inserted );
		}

		size_type erase( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			size_type count = 0;
			_elements.remove_if([&key, &count](value_type value){ auto tmp = (value.first == key); count += tmp; return tmp; });
			return count;
		}
};

} // namespace detail
} // namespace unordered

#endif // HASH_TRIE_DETAIL_NODE
