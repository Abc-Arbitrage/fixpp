/* list.h
   Mathieu Stefani, 13 may 2017
   
  A compile-time type-list
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

				// Used for Find so that we can use std::conditional which works on types
                template<int I> struct Int
                {
                    static constexpr int Value = I;
                };

                template<int, typename, template<typename> class Pred>
                struct Find;

                template<int Index, template<typename> class Pred>
                struct Find<Index, NullType, Pred>
                {
                    using ResultT = Int<-1>;
                };

                template<int Index, typename Head, typename Tail, template<typename> class Pred>
                struct Find<Index, TypeList<Head, Tail>, Pred>
                {
                    using ResultT = typename std::conditional<
                        Pred<Head>::value,
                        Int<Index>,
                        typename Find<Index + 1, Tail, Pred>::ResultT
                    >::type;

                    static constexpr int Result = ResultT::Value;
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

			template<typename List, template<typename> class Pred>
            struct Find : public impl::Find<0, List, Pred>
            {
            };

            template<template<typename> class Pred>
            struct Find<NullType, Pred>
            {
                static constexpr int Result = -1;
            };

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

} // namespace meta
