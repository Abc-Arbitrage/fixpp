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

} // namespace ops
} // namespace typelist

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

} // namespace meta
