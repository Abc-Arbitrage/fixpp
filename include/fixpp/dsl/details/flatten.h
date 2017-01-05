/* message.h
   Mathieu Stefani, 05 january 2017
   
*/

#pragma once

namespace Fix
{

    namespace details
    {

        namespace flatten
        {

            namespace tuple
            {

                template<typename T1, typename T2> struct TupleCat;

                template<typename... Args1, typename... Args2>
                struct TupleCat<std::tuple<Args1...>, std::tuple<Args2...>>
                {
                    using Result = std::tuple<Args1..., Args2...>;
                };

                template<template<typename> class FieldT, typename... Tags>
                struct Flatten;

                template<template<typename> class FieldT, typename T>
                struct FlattenSingle
                {
                    using Result = std::tuple<FieldT<T>>;
                };

                template<template<typename> class FieldT, typename Head, typename... Tail>
                struct FlattenSingle<FieldT, ComponentBlock<Head, Tail...>>
                {
                    using Result = typename TupleCat<
                                      typename FlattenSingle<FieldT, typename Unwrap<Head>::Result>::Result,
                                      typename Flatten<FieldT, Tail...>::Result
                                   >::Result;
                };


                template<template<typename> class FieldT, typename Head, typename... Tail>
                struct Flatten<FieldT, Head, Tail...>
                {
                    using Result = typename TupleCat<
                                      typename FlattenSingle<FieldT, typename Unwrap<Head>::Result>::Result,
                                      typename Flatten<FieldT, Tail...>::Result
                                   >::Result;
                };

                template<template<typename> class FieldT>
                struct Flatten<FieldT>
                {
                    using Result = std::tuple<>;
                };

            } // namespace tuple

            namespace typelist
            {
                template<typename... Tags>
                struct Flatten;

                template<typename T>
                struct FlattenSingle
                {
                    using Result = typename meta::typelist::make<T>::Result;
                };

                template<typename Head, typename... Tail>
                struct FlattenSingle<ComponentBlock<Head, Tail...>>
                {
                    using Result = typename meta::typelist::ops::Append<
                                        typename FlattenSingle<Head>::Result,
                                        typename Flatten<Tail...>::Result
                                    >::Result;
                };

                template<typename Head, typename... Tail>
                struct Flatten<Head, Tail...>
                {
                    using Result = typename meta::typelist::ops::Append<
                                       typename FlattenSingle<Head>::Result,
                                       typename Flatten<Tail...>::Result
                                   >::Result;
                };

                template<>
                struct Flatten<>
                {
                    using Result = typename meta::typelist::make<>::Result;
                };

            } // namespace typelist

        } // namespace flatten

        template<template<typename> class FieldT, typename... Tags>
        struct Flattened
        {
            using Fields = typename flatten::tuple::Flatten<FieldT, Tags...>::Result;
            using List = typename flatten::typelist::Flatten<Tags...>::Result;
        };

    } // namespace details

} // namespace Fix