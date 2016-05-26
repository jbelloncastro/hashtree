
#ifndef HASH_TRIE_DETAIL_UTILS
#define HASH_TRIE_DETAIL_UTILS

namespace unordered {
namespace detail {
namespace utils {

constexpr size_t power( size_t base, size_t exp )
{
	return exp==0?1: base*power(base,exp-1);
}

template< typename Integral >
constexpr Integral log( Integral base, Integral val )
{
	return val<=1? 0 : 1 + log( base, val/base );
}

} // namespace utils
} // namespace detail
} // namespace unordered

#endif // HASH_TRIE_DETAIL_TRAITS

