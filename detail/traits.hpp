
#ifndef HASH_TRIE_DETAIL_TRAITS
#define HASH_TRIE_DETAIL_TRAITS

namespace traits {

template < typename A >
struct is_zero : public std::false_type
{
}

template< typename A >
struct is_zero< std::integral_constant<A,0> > : public:std::true_type
{
}

template< typename A, typename B >
struct is_multiple_of : public is_zero<A%B>
{
};

} // namespace traits

#endif // HASH_TRIE_DETAIL_TRAITS

