/* meta.h
   Mathieu Stefani, 12 november 2016
   
    A collection of metaprogramming utility
*/

#pragma once

namespace meta
{

    namespace typelist
    {

        struct NullType {};

        template <typename T, typename U>
        struct TypeList {
            using Head = T;
            using Tail = U;
        };

        template <typename...> struct make;

        template <>
        struct make<> {
            using Result = NullType;
        };

        template <typename Head, typename... Types>
        struct make<Head, Types...> {
            using Result = TypeList<Head, typename make<Types...>::Result>;
        };

        namespace ops
        {
            template <typename> struct Length;

            template <>
            struct Length<NullType> {
                enum { value = 0 };
            };

            template <typename Head, typename Tail>
            struct Length<TypeList<Head, Tail>> {
                enum { value = Length<Tail>::value + 1 };
            };

            template <typename, typename> struct IndexOf;

            template <typename T>
            struct IndexOf<NullType, T> {
                enum { value = -1 };
            };

            template <typename T, typename Tail>
            struct IndexOf<TypeList<T, Tail>, T> {
                enum { value = 0 };
            };

            template <typename Head, typename Tail, typename T>
            struct IndexOf<TypeList<Head, Tail>, T> {
                using Result = IndexOf<Tail, T>;
                enum { value = Result::value == -1 ? -1 : Result::value + 1 };
            };

            namespace impl {

                template<size_t, size_t, typename> struct At;

                template<size_t Index, size_t Current>
                struct At<Index, Current, NullType>
                {
                    using Result = NullType;
                };

                template<size_t Index, size_t Current, typename Head, typename Tail>
                struct At<Index, Current, TypeList<Head, Tail>>
                {
                    using Result = typename At<Index, Current + 1, Tail>::Result;
                };

                template<size_t Index, typename Head, typename Tail>
                struct At<Index, Index, TypeList<Head, Tail>>
                {
                    using Result = Head;
                };
            }

            template<size_t Index, typename List>
            struct At : public impl::At<Index, 0, List>
            {
            };

            template<typename List>
            using First = At<0, List>;

            template<typename List>
            using Last = At<Length<List>::value - 1, List>;

            template<typename, template<typename> class Pred> struct Filter;

            template<template<typename> class Pred>
            struct Filter<NullType, Pred>
            {
                using Result = NullType;
            };

            template<typename Head, typename Tail, template<typename> class Pred>
            struct Filter<TypeList<Head, Tail>, Pred>
            {
                using Result = typename std::conditional<
                        Pred<Head>::value,
                        TypeList<Head, typename Filter<Tail, Pred>::Result>,
                        typename Filter<Tail, Pred>::Result
                      >::type;
            };

            template<typename, template<typename> class Op> struct Map;

            template<template<typename> class Op>
            struct Map<NullType, Op>
            {
                using Result = NullType;
            };

            template<typename Head, typename Tail, template<typename> class Op>
            struct Map<TypeList<Head, Tail>, Op>
            {
                using Result = TypeList<
                                  typename Op<Head>::Result,
                                  typename Map<Tail, Op>::Result
                               >;
            };

            template<typename List1, typename List2>
            struct Append;

            template<typename Head1, typename Tail1, typename List2>
            struct Append<TypeList<Head1, Tail1>, List2>
            {
                using Result = TypeList<Head1, typename Append<Tail1, List2>::Result>;
            };

            template<typename List2>
            struct Append<NullType, List2>
            {
                using Result = List2;
            };

            template<typename, typename Ret, Ret Result, template<typename, Ret> class Op>
            struct Fold;

            template<typename Ret, Ret Result, template<typename, Ret> class Op>
            struct Fold<NullType, Ret, Result, Op>
            {
                static constexpr Ret Value = Result;
            };

            template<typename Head, typename Tail, typename Ret, Ret Result, template<typename, Ret> class Op>
            struct Fold<TypeList<Head, Tail>, Ret, Result, Op>
            {
                static constexpr Ret Value = Fold<Tail, Ret, Op<Head, Result>::Result, Op>::Value;
            };

            template<typename List, typename VisitorT>
            struct Visitor;

            template<typename Head, typename Tail, typename VisitorT>
            struct Visitor<TypeList<Head, Tail>, VisitorT>
            {
                template<typename... Args>
                static bool visit(Args&& ...args)
                {
                    if (VisitorT::template visit<Head>(std::forward<Args>(args)...))
                        return true;

                    return Visitor<Tail, VisitorT>::visit(std::forward<Args>(args)...);
                }
            };

            template<typename VisitorT>
            struct Visitor<NullType, VisitorT>
            {
                template<typename... Args>
                static bool visit(Args&& ...)
                {
                    return false;
                }
            };

        } // namespace ops

    } // namespace typelist

    namespace map
    {

        template<typename First, typename Second> struct Pair { };
        template<typename... Pairs> struct Map { };

        namespace ops
        {

            template<typename Map, typename Pair> struct insert;

            template<typename... Pairs, typename Pair>
            struct insert<Map<Pairs...>, Pair>
            {
                using type = Map<Pairs..., Pair>;
            };

            template<typename Map, typename Key> struct at;

            template<typename First, typename Second, typename... Rest, typename Key>
            struct at<Map<Pair<First, Second>, Rest...>, Key>
            {
                using type = typename at<Map<Rest...>, Key>::type;
            };

            template<typename First, typename Second, typename... Rest>
            struct at<Map<Pair<First, Second>, Rest...>, First>
            {
                using type = Second;
            };

            template<typename Key>
            struct at<Map<>, Key>
            {
            };

            template<typename Map, typename Key, typename Default> struct atOr;

            template<typename First, typename Second, typename... Rest, typename Key, typename Default>
            struct atOr<Map<Pair<First, Second>, Rest...>, Key, Default>
            {
                using type = typename atOr<Map<Rest...>, Key, Default>::type;
            };

            template<typename First, typename Second, typename... Rest, typename Default>
            struct atOr<Map<Pair<First, Second>, Rest...>, First, Default>
            {
                using type = Second;
            };

            template<typename Key, typename Default>
            struct atOr<Map<>, Key, Default>
            {
                using type = Default;
            };

        } // namespace ops

    } // namespace map

    namespace pack
    {

        template<typename... Args>
        struct Pack
        { };

        namespace ops
        {

            template<typename Pack, typename Value>
            struct Append;

            template<typename... Vals, typename Value>
            struct Append<Pack<Vals...>, Value>
            {
                using Result = Pack<Vals..., Value>;
            };

            template<typename... Vals, typename... Others>
            struct Append<Pack<Vals...>, Pack<Others...>>
            {
                using Result = Pack<Vals..., Others...>;
            };

        } // namespace ops

    } // namespace pack

    template <size_t... Ints>
    struct index_sequence
    {
        using type = index_sequence;
        using value_type = size_t;
        static constexpr std::size_t size() noexcept { return sizeof...(Ints); }
    };

    // --------------------------------------------------------------

    template <class Sequence1, class Sequence2>
    struct _merge_and_renumber;

    template <size_t... I1, size_t... I2>
    struct _merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
      : index_sequence<I1..., (sizeof...(I1) + I2)...>
    { };

    // --------------------------------------------------------------

    template <size_t N>
    struct make_index_sequence
      : _merge_and_renumber<typename make_index_sequence<N/2>::type,
                            typename make_index_sequence<N - N/2>::type>
    { };

    template<> struct make_index_sequence<0> : index_sequence<> { };
    template<> struct make_index_sequence<1> : index_sequence<0> { };

    template<size_t N>
    using make_index_sequence_t = typename make_index_sequence<N>::type;

    template<typename T>
    T max(T&& value)
    {
        return std::forward<T>(value);
    }

    template<typename T0, typename T1, typename... Ts>
    constexpr typename std::common_type<
        T0, T1, Ts...
    >::type max(T0&& t0, T1&& t1, Ts&& ...rest)
    {
        return (t0 > t1) ?
            max(t0, std::forward<Ts>(rest)...) :
            max(t1, std::forward<Ts>(rest)...);
    }


} // namespace meta

namespace meta2
{
    template<class T, T... Ints> struct integer_sequence
    {
    };

    template<class S> struct next_integer_sequence;

    template<class T, T... Ints> struct next_integer_sequence<integer_sequence<T, Ints...>>
    {
            using type = integer_sequence<T, Ints..., sizeof...(Ints)>;
    };

    template<class T, T I, T N> struct make_int_seq_impl;

    template<class T, T N>
            using make_integer_sequence = typename make_int_seq_impl<T, 0, N>::type;

    template<class T, T I, T N> struct make_int_seq_impl
    {
            using type = typename next_integer_sequence<
                        typename make_int_seq_impl<T, I+1, N>::type>::type;
    };

    template<class T, T N> struct make_int_seq_impl<T, N, N>
    {
            using type = integer_sequence<T>;
    };

    template<std::size_t... Ints>
            using index_sequence = integer_sequence<std::size_t, Ints...>;

    template<std::size_t N>
            using make_index_sequence = make_integer_sequence<std::size_t, N>;
}
