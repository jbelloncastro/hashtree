
#ifndef HASH_TRIE_DETAIL_BUCKET
#define HASH_TRIE_DETAIL_BUCKET

#include "utils.hpp"

namespace unordered {
namespace detail {

template<
    class Types
> class bucket_element {
	public:
		typedef Types::key_type key_type;
		typedef Types::value_type value_type;
		typedef Types::reference reference;
		typedef Types::const_reference const_reference;
		typedef Types::Allocator allocator_type;

	private:
		Allocator       _allocator;
		bucket_element* _next;
		pointer_type    _value;

	public:
		// Default construction
		bucket_element( const key_type& key, const Allocator& allocator = Allocator() ) :
			_allocator( allocator )
			_next( nullptr ),
			_value( _allocator.allocate(1) )
		{
			_allocator.construct( _value, std::piecewise_construct, std::forward_as_tuple(k), std::tuple<>() );
		}

		// in-place construction
		template< class... Args >
		bucket_element( Args...&& args, const Allocator& allocator = Allocator() ) :
			_allocator( allocator )
			_next( nullptr ),
			_value( _allocator.allocate(1) )
		{
			_allocator.construct( _value, std::forward(args)... );
		}

		// move constructor
		bucket_element( bucket_element&& other ) :
			_allocator( std::move( other._allocator ) ),
			_next( std::move( other._next ) ),
			_value( std::move( other._value ) )
		{
			other._next = nullptr;
		}

		~bucket_element()
		{
			if( _next )
				delete _next;
		}

		template< class... Args >
		void emplace_back( Args..&& args )
		{
			_next = new bucket_element( std::forward(args)..., _allocator );
		}

		bucket_element* next()
		{
			return _next;
		}

		const bucket_element* next() const
		{
			return _next;
		}

		reference value()
		{
			return *_value;
		}

		const_reference value() const
		{
			return *_value;
		}
};

template<
    class Types
> class bucket {
	public:
		typedef Types::value_type value_type;
		typedef Types::reference reference;
		typedef Types::const_reference const_reference;
		typedef Types::Allocator Allocator;

	private:
		bucket_element* _front;
		bucket_element* _back;

	public:
		// default constructor: at least size must be 1
		template < class... Args >
		bucket( const Args...& args ) :
			_front( new bucket_element(std::forward(args)...) ),
			_back( _front )
		{
		}

		// destructor
		~bucket()
		{
			delete _front;
			_back = nullptr;
		}

		std::size_t size() const
		{
			std::size_t result = 1;
			bucket_element* current = _front;
			while( _front != _back )
				result++;
			return result;
		}
};

} // namespace detail
} // namespace unordered

#endif // HASH_TRIE_DETAIL_BUCKET

