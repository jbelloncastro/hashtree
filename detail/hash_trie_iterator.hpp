#ifndef __HASH_TRIE_ITERATOR_HPP__
#define __HASH_TRIE_ITERATOR_HPP__

#include <memory>

namespace unordered {
namespace detail {

template <
    class Types
> class hash_trie_iterator : public std::iterator< std::forward_iterator_tag, typename Types::value_type >
{
public:
	typedef typename Types::value_type value_type;
	typedef typename Types::value_type* pointer;
	typedef typename Types::reference reference;
	typedef typename Types::const_reference const_reference;
	typedef typename Types::value_list::iterator l2_iterator;
	typedef typename Types::leaf_list list;
	typedef typename list::iterator l1_iterator;
	
	hash_trie_iterator( list& l, const l1_iterator& it1, const l2_iterator& it2 ) : _list( &l ), _it1( it1 ), _it2( it2 ) {}
	hash_trie_iterator( list& l, const l1_iterator& it1 ) : hash_trie_iterator( l, it1, it1 == l.cend() ? l2_iterator( nullptr ) : (*it1)->begin() ) {}
	hash_trie_iterator( list& l ) : hash_trie_iterator( l, l.cend() ) {}

	hash_trie_iterator operator++()
	{
		++_it2;
		while ( _it2 == ( *_it1 )->end() ) {
			++_it1;
			if ( _it1 != _list->end() ) {
				_it2 = ( *_it1 )->begin();
			} else {
				_it2 = l2_iterator( nullptr );
				break;
			}
		}
		return *this;
	}

	reference operator*() { return *_it2; }
	pointer operator->() { return &(*_it2); }

	bool operator==(const hash_trie_iterator& rhs) { return  _it1 == rhs._it1 && _it2 == rhs._it2; }
	bool operator!=(const hash_trie_iterator& rhs) { return  _it1 != rhs._it1 || _it2 != rhs._it2; }

private:
	typedef list* list_ptr;
	
	list_ptr	_list;
	l1_iterator	_it1;
	l2_iterator 	_it2;
};

} // namespace detail
} // namespace unordered

#endif /* __HASH_TRIE_ITERATOR_HPP__ */
