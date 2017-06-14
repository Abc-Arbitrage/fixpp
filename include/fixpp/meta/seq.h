/* seq.h
   Mathieu Stefani, 13 may 2017
   
  A compile-time sequence of integers
*/

// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>

#if __cplusplus >= 201402L
# define FIXPP_META_USE_STD_INTEGER_SEQUENCE
#endif

#if (__cplusplus >= 201402L) && defined(_LIBCPP_VERSION)
# define FIXPP_META_USE_STD_MAKE_INTEGER_SEQUENCE
#endif

#if defined(__cpp_fold_expressions)
# define FIXPP_META_FOLD_EXPRESSIONS
#elif __cplusplus > 201402L
# if defined(__apple_build_version__) && (__clang_major__ >= 7)
#  define FIXPP_META_FOLD_EXPRESSIONS
# elif defined(__clang__) && ((__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ >= 6)))
#  define FIXPP_META_FOLD_EXPRESSIONS
# endif
#endif



namespace meta
{
    namespace seq
    {

    // ------------------------------------------------
    // integer_sequence
    // ------------------------------------------------

#ifdef FIXPP_META_USE_STD_INTEGER_SEQUENCE

    using std::integer_sequence;
    using std::index_sequence;

#else

    template< typename T, T... Ns >
    struct integer_sequence
    {
      using value_type = T;

      static constexpr std::size_t size() noexcept
      {
        return sizeof...( Ns );
      }
    };

    template< std::size_t... Ns >
    using index_sequence = integer_sequence< std::size_t, Ns... >;

#endif

    // ------------------------------------------------
    // make_integer_sequence
    // ------------------------------------------------

#ifdef FIXPP_META_USE_STD_MAKE_INTEGER_SEQUENCE

    using std::make_integer_sequence;
    using std::make_index_sequence;
    using std::index_sequence_for;

#else

    // idea from http://stackoverflow.com/a/13073076

    namespace impl
    {
      template< typename, std::size_t, bool >
      struct double_up;

      template< typename T, T... Ns, std::size_t N >
      struct double_up< integer_sequence< T, Ns... >, N, false >
      {
        using type = integer_sequence< T, Ns..., ( N + Ns )... >;
      };

      template< typename T, T... Ns, std::size_t N >
      struct double_up< integer_sequence< T, Ns... >, N, true >
      {
        using type = integer_sequence< T, Ns..., ( N + Ns )..., 2 * N >;
      };

      template< typename T, T N, typename = void >
      struct generate_sequence;

      template< typename T, T N >
      using generate_sequence_t = typename generate_sequence< T, N >::type;

      template< typename T, T N, typename >
      struct generate_sequence
        : double_up< generate_sequence_t< T, N / 2 >, N / 2, N % 2 == 1 >
      {};

      template< typename T, T N >
      struct generate_sequence< T, N, typename std::enable_if< ( N == 0 ) >::type >
      {
        using type = integer_sequence< T >;
      };

      template< typename T, T N >
      struct generate_sequence< T, N, typename std::enable_if< ( N == 1 ) >::type >
      {
        using type = integer_sequence< T, 0 >;
      };
    }

    template< typename T, T N >
    using make_integer_sequence = impl::generate_sequence_t< T, N >;

    template< std::size_t N >
    using make_index_sequence = make_integer_sequence< std::size_t, N >;

    template< typename... Ts >
    using index_sequence_for = make_index_sequence< sizeof...( Ts ) >;

#endif

    // ------------------------------------------------
    // is_all
    // ------------------------------------------------

	template< bool... Bs >
    using is_all = std::integral_constant< bool, std::is_same< integer_sequence< bool, true, Bs... >, integer_sequence< bool, Bs..., true > >::value >;
	
    // ------------------------------------------------
    // sum
    // ------------------------------------------------

	namespace impl
    {
      template< std::size_t, std::size_t N >
      struct chars
      {
        char dummy[ N + 1 ];
      };

      template< typename, std::size_t... >
      struct collector;

      template< std::size_t... Is, std::size_t... Ns >
      struct collector< index_sequence< Is... >, Ns... >
        : chars< Is, Ns >...
      {};

      template< std::size_t N, typename T, T... Ns >
      struct sum
      {
          using type = std::integral_constant<
          T,
          T( sizeof( collector< make_index_sequence< N >, ( ( Ns > 0 ) ? Ns : 0 )... > ) - N ) -
          T( sizeof( collector< make_index_sequence< N >, ( ( Ns < 0 ) ? -Ns : 0 )... > ) - N ) >;
      };
    }

    template< typename T, T... Ns >
    struct sum
      : impl::sum< sizeof...( Ns ) + 1, T, Ns..., 0 >::type
    {};

	template< typename T, T... Ns >
    struct sum< integer_sequence< T, Ns... > >
      : sum< T, Ns... >
    {};

    // ------------------------------------------------
    // partial_sum
    // ------------------------------------------------

	namespace impl
    {
      template< std::size_t, typename S, typename = make_index_sequence< S::size() > >
      struct partial_sum;

      template< std::size_t I, typename T, T... Ns, std::size_t... Is >
      struct partial_sum< I, integer_sequence< T, Ns... >, index_sequence< Is... > >
        : seq::sum< T, ( ( Is < I ) ? Ns : 0 )... >
      {
        static_assert( I <= sizeof...( Is ), "tao::seq::partial_sum<I, S>: I is out of range" );
      };
    }

    template< std::size_t I, typename T, T... Ns >
    struct partial_sum
      : impl::partial_sum< I, integer_sequence< T, Ns... > >
    {};

    template< std::size_t I, typename T, T... Ns >
    struct partial_sum< I, integer_sequence< T, Ns... > >
      : impl::partial_sum< I, integer_sequence< T, Ns... > >
    {};

    // ------------------------------------------------
    // exclusive_scan
    // ------------------------------------------------

	namespace impl
    {
      template< typename S, typename = make_index_sequence< S::size() > >
      struct exclusive_scan;

      template< typename S, std::size_t... Is >
      struct exclusive_scan< S, index_sequence< Is... > >
      {
        using type = integer_sequence< typename S::value_type, partial_sum< Is, S >::value... >;
      };
    }

    template< typename T, T... Ns >
    struct exclusive_scan
      : impl::exclusive_scan< integer_sequence< T, Ns... > >
    {};

    template< typename T, T... Ns >
    struct exclusive_scan< integer_sequence< T, Ns... > >
      : impl::exclusive_scan< integer_sequence< T, Ns... > >
    {};

    template< typename T, T... Ns >
    using exclusive_scan_t = typename exclusive_scan< T, Ns... >::type;

    // ------------------------------------------------
    // zip
    // ------------------------------------------------

	template< template< typename U, U, U > class, typename, typename >
    struct zip;

    template< template< typename U, U, U > class OP, typename TA, TA... As, typename TB, TB... Bs >
    struct zip< OP, integer_sequence< TA, As... >, integer_sequence< TB, Bs... > >
    {
      using CT = typename std::common_type< TA, TB >::type;
      using type = integer_sequence< CT, OP< CT, As, Bs >::value... >;
    };

    template< template< typename U, U, U > class OP, typename A, typename B >
    using zip_t = typename zip< OP, A, B >::type;

    // ------------------------------------------------
    // plus
    // ------------------------------------------------

	namespace impl
    {
      template< typename T, T A, T B >
      using plus = std::integral_constant< T, A + B >;
    }

    template< typename A, typename B >
    using plus = zip< impl::plus, A, B >;

    template< typename A, typename B >
    using plus_t = typename plus< A, B >::type;

    // ------------------------------------------------
    // inclusive_scan
    // ------------------------------------------------

	template< typename T, T... Ns >
    struct inclusive_scan
      : plus< exclusive_scan_t< T, Ns... >, integer_sequence< T, Ns... > >
    {};

    template< typename T, T... Ns >
    struct inclusive_scan< integer_sequence< T, Ns... > >
      : plus< exclusive_scan_t< integer_sequence< T, Ns... > >, integer_sequence< T, Ns... > >
    {};

    template< typename T, T... Ns >
    using inclusive_scan_t = typename inclusive_scan< T, Ns... >::type;

    // ------------------------------------------------
    // fold
    // ------------------------------------------------

	template< typename T, T... Ns >
    struct values
    {
      static constexpr T data[] = { Ns... };
    };

	namespace impl
    {
      template< template< typename U, U, U > class, typename, bool, typename T, T... >
      struct folder;

      template< template< typename U, U, U > class OP, std::size_t... Is, typename T, T... Ns >
      struct folder< OP, index_sequence< Is... >, false, T, Ns... >
      {
        using values = seq::values< T, Ns... >;
        using type = integer_sequence< T, OP< T, values::data[ 2 * Is ], values::data[ 2 * Is + 1 ] >::value... >;
      };

      template< template< typename U, U, U > class OP, std::size_t... Is, typename T, T N, T... Ns >
      struct folder< OP, index_sequence< Is... >, true, T, N, Ns... >
      {
        using values = seq::values< T, Ns... >;
        using type = integer_sequence< T, N, OP< T, values::data[ 2 * Is ], values::data[ 2 * Is + 1 ] >::value... >;
      };
    }

    template< template< typename U, U, U > class, typename T, T... >
    struct fold;

    template< template< typename U, U, U > class OP, typename T, T N >
    struct fold< OP, T, N >
      : std::integral_constant< T, N >
    {};

    template< template< typename U, U, U > class OP, typename T, T... Ns >
    struct fold
      : fold< OP, typename impl::folder< OP, make_index_sequence< sizeof...( Ns ) / 2 >, sizeof...( Ns ) % 2 == 1, T, Ns... >::type >
    {};

    template< template< typename U, U, U > class OP, typename T, T... Ns >
    struct fold< OP, integer_sequence< T, Ns... > >
      : fold< OP, T, Ns... >
    {};

    // ------------------------------------------------
    // max
    // ------------------------------------------------

	namespace impl
    {
      template< typename T, T A, T B >
      using max = std::integral_constant< T, ( ( A > B ) ? A : B ) >;
    }

    template< typename T, T... Ns >
    struct max
      : fold< impl::max, T, Ns... >
    {};

    template< typename T, T... Ns >
    struct max< integer_sequence< T, Ns... > >
      : max< T, Ns... >
    {};

    // ------------------------------------------------
    // min
    // ------------------------------------------------

	namespace impl
    {
      template< typename T, T A, T B >
      using min = std::integral_constant< T, ( ( A < B ) ? A : B ) >;
    }

    template< typename T, T... Ns >
    struct min
      : fold< impl::min, T, Ns... >
    {};

    template< typename T, T... Ns >
    struct min< integer_sequence< T, Ns... > >
      : min< T, Ns... >
    {};

    // ------------------------------------------------
    // minus
    // ------------------------------------------------

	namespace impl
    {
      template< typename T, T A, T B >
      using minus = std::integral_constant< T, A - B >;
    }

    template< typename A, typename B >
    using minus = zip< impl::minus, A, B >;

    template< typename A, typename B >
    using minus_t = typename minus< A, B >::type;

    // ------------------------------------------------
    // type_by_index
    // ------------------------------------------------

#if 0

    namespace impl
    {
        template<std::size_t N, std::size_t Index, typename... Ts>
        struct get_nth;

        template<std::size_t N, std::size_t Index, typename Head, typename... Tail>
        struct get_nth<N, Index, Head, Tail...>
        {
            using type = typename get_nth<N, Index + 1, Tail...>::type;
        };

        template<std::size_t Index, typename Head, typename... Tail>
        struct get_nth<Index, Index, Head, Tail...>
        {
            using type = Head;
        };

    }

    template< std::size_t I, typename... Ts >
    using type_by_index = impl::get_nth<I, 0, Ts...>;

    template< std::size_t I, typename... Ts >
    using type_by_index_t = typename type_by_index< I, Ts... >::type;

#else

	// based on http://stackoverflow.com/questions/18942322

    namespace impl
    {
      template< std::size_t >
      struct any
      {
        any( ... );
      };

      template< typename >
      struct wrapper;

      template< typename >
      struct unwrap;

      template< typename T >
      struct unwrap< wrapper< T > >
      {
        using type = T;
      };

      template< typename >
      struct get_nth;

      template< std::size_t... Is >
      struct get_nth< index_sequence< Is... > >
      {
        template< typename T >
        static T deduce( any< Is & 0 >..., T*, ... );
      };

    }

	template< std::size_t I, typename... Ts >
	using type_by_index = impl::unwrap< decltype(impl::get_nth<make_index_sequence< I > >::deduce(std::declval<impl::wrapper< Ts >* >()...)) >;

    template< std::size_t I, typename... Ts >
    using type_by_index_t = typename type_by_index< I, Ts... >::type;

#endif

    // ------------------------------------------------
    // select
    // ------------------------------------------------

	template< std::size_t I, typename T, T... Ns >
    struct select
      : std::integral_constant< T, values< T, Ns... >::data[ I ] >
    {};

    template< std::size_t I, typename T, T... Ns >
    struct select< I, integer_sequence< T, Ns... > >
      : select< I, T, Ns... >
    {};

    // ------------------------------------------------
    // map
    // ------------------------------------------------

	template< typename, typename >
    struct map;

    template< std::size_t... Ns, typename M >
    struct map< index_sequence< Ns... >, M >
    {
      using type = integer_sequence< typename M::value_type, select< Ns, M >::value... >;
    };

    template< typename S, typename M >
    using map_t = typename map< S, M >::type;

} // namespace seq
} // namespace meta
