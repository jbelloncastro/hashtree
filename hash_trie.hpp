#ifndef HASH_TRIE
#define HASH_TRIE

#include "detail/node.hpp"
#include "detail/utils.hpp"
#include "detail/hash_trie_iterator.hpp"

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
		typedef std::size_t size_type;		//< Unsigned integral type (usually std::size_t)
		typedef std::ptrdiff_t difference_type;	//< Signed integer type (usually std::ptrdiff_t)
		typedef Hash hasher;
		typedef KeyEqual key_equal;
		typedef Allocator allocator_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename std::allocator_traits<Allocator>::pointer pointer_type;
		typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer_type;
		typedef typename Hash::result_type hash_type;
		typedef hash_trie_iterator<hash_trie> iterator;
		typedef const iterator const_iterator;
		
		typedef typename std::forward_list< value_type > value_list;
		typedef value_list* value_list_ptr;
		typedef typename std::forward_list< value_list_ptr > leaf_list;


	private:
		static constexpr size_t hash_step = log( ( hash_type )( 2 ), CACHE_LINE_SIZE/sizeof( hash_type ));
		static constexpr size_t hash_offset = sizeof( hash_type )-hash_step;
		typedef node<
			hash_trie,
			hash_step,
			hash_offset
		> node_type;

		node_type	_root;
                size_type	_elemCount;
		Hash		_hasher;
		leaf_list	_elems;
	public:
		hash_trie() : _root(), _elemCount( 0 ), _hasher(), _elems() {}

		size_type size() const
		{
			return _elemCount;
		}

		std::pair<iterator,bool> insert( const value_type& value )
		{
			std::pair<iterator,bool> ret = _root.insert( _hasher( value.first ), value, _elems );
			_elemCount += ret.second;
			return ret;
		}

		iterator erase ( const_iterator position ) {
			_elemCount--;
			return position.erase();
		}

		iterator find ( const key_type& k )
		{
			return _root.find( _hasher( k ), k, _elems );
		}

		mapped_type& operator[] ( const key_type& k )
		{
			return _root.get( _hasher( k ), k, _elems ).second;
		}

		iterator begin()
		{
			return iterator( _elems, _elems.begin() );
		}
		
		const_iterator cbegin()
		{
			return begin();
		}
		
		iterator end()
		{
			return iterator( _elems, _elems.end() );
		}

		const_iterator cend()
		{
			return end();
		}
};

} // namespace unordered

#endif // HASH_TRIE
