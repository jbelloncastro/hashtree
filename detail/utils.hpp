
#ifndef HASH_TRIE_DETAIL_UTILS
#define HASH_TRIE_DETAIL_UTILS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
// meanwhile no config.h
#define CACHE_LINE_SIZE 128

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

template< typename Integral >
constexpr Integral min( Integral x, Integral y )
{
	return x < y ? x : y;
}

template< typename Integral >
constexpr Integral max( Integral x, Integral y )
{
	return x > y ? x : y;
}

} // namespace utils
} // namespace detail
} // namespace unordered

#endif // HASH_TRIE_DETAIL_TRAITS
