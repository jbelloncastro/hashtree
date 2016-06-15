#ifndef HASH_TRIE_DETAIL_NODE
#define HASH_TRIE_DETAIL_NODE

#include "utils.hpp"

#include<iostream>
#include<memory>

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

		typedef next_node_type* next_node_pointer;
		typedef std::atomic< next_node_pointer > next_node_pointer_atomic;

		typedef std::array< next_node_pointer_atomic, power(2, hash_step) > table;

		table _nextNodes;

	public:
		node() : _nextNodes()
		{
			for ( size_t i = 0; i < _nextNodes.size(); ++i ) {
				_nextNodes[i] = nullptr;
			}
		}

		node( leaf_list& list ) : node() {}

		void push_list( leaf_list& list ) {}

		iterator find( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			next_node_pointer next = _nextNodes[ compute_index( hash_value ) ];
			return next ? next->find( hash_value, key, list ) : iterator( list );
		}

		size_type compute_index( hash_value_t hash_value )
		{
			return (hash_value >> hash_offset) & ((1<<hash_step)-1);
		}

		std::pair<iterator,bool> insert( const hash_value_t& hash_value, const value_type& value, leaf_list& list )
		{
			size_type index = compute_index( hash_value );
			next_node_pointer next = _nextNodes[index];
			if( !next ) {
				next_node_pointer newNext = new next_node_type( list );
				if ( !_nextNodes[index].compare_exchange_strong( next, newNext ) ) {
					delete newNext;
				} else {
					newNext->push_list( list );
				}
				next = _nextNodes[index];
			}
			return next->insert( hash_value, value, list );
		}

		std::pair<iterator,bool> get( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			size_type index = compute_index( hash_value );
			next_node_pointer next = _nextNodes[index];
			if( !next ) {
				next_node_pointer newNext = new next_node_type( list );
				if ( !_nextNodes[index].compare_exchange_strong( next, newNext ) ) {
					delete newNext;
				} else {
					newNext->push_list( list );
				}
				next = _nextNodes[index];
			}
			return next->get( hash_value, key, list );
		}

		size_type erase( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			next_node_pointer next = _nextNodes[ compute_index( hash_value ) ];
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
		node( leaf_list& list ) : _elements(), _self() {}

		void push_list( leaf_list& list ) {
			_self = list.insert_after( list.before_begin(), &_elements );
		}

		trie_iterator find( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			auto it = _elements.begin();
			while ( it != _elements.end() && it->first != key ) ++it;
			return it == _elements.end() ? trie_iterator( list ) : trie_iterator( list, _self, it );
		}

		std::pair<trie_iterator,bool> insert( const hash_value_t& hash_value, const value_type& value, leaf_list& list )
		{
			auto it = _elements.before_begin();
			auto itNext = _elements.begin();
			bool inserted = false;
			do {
				while ( itNext != _elements.end() && itNext->first != value.first ) { ++it; ++itNext; }
				if ( itNext == _elements.end() ) {
					itNext = _elements.try_insert_after( it, itNext, value );
					inserted = itNext != _elements.end();
					if ( !inserted ) {
						itNext = it;
						++itNext;
					}
				}
			} while ( itNext == _elements.end() );
			trie_iterator it2 = trie_iterator( list, _self, itNext );
			return std::make_pair( it2, inserted );
		}

		std::pair<trie_iterator,bool> get( const hash_value_t& hash_value, const key_type& key, leaf_list& list )
		{
			auto it = _elements.before_begin();
			auto itNext = _elements.begin();
			bool inserted = false;
			do {
				while ( itNext != _elements.end() && itNext->first != key ) { ++it; ++itNext; }
				if ( itNext == _elements.end() ) {
					itNext = _elements.try_emplace_after( it, itNext, key, mapped_type() );
					inserted = itNext != _elements.end();
					if ( !inserted ) {
						itNext = it;
						++itNext;
					}
				}
			} while ( itNext == _elements.end() );
			trie_iterator it2 = trie_iterator( list, _self, itNext );
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
