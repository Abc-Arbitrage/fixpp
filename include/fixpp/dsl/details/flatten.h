/* message.h
   Mathieu Stefani, 05 january 2017

   A collection of utilities to "flatten" a variadic list of template arguments.

   Since the DSL exposes types like ComponentBlock<Args...> to define FIX Component
   Blocks, let's suppose we have the following component:

   using Component = ComponentBlock<C, D, E>;

   And then we have the following Message:

   using Message = MessageT<'0', A, B, Component>;

   The final type of Message will then be:

   MessageT<
      '0',
      A, B,
      ComponentBlock<C, D, E>
  >

  Since fields are stored inside a std::tuple, we will then have
  std::tuple<Field<A>, Field<B>, Field<ComponentBlock<C, D, E>>> which we do not really
  want. Instead, we want std::tuple<Field<A>, Field<B>, Field<C>, Field<D>, Field<E>>, that
  is to say everything inside a ComponentBlock must be "flatten" inside the final tuple.

  That's why we have special logic to flatten everything out.
  Also note that we need to handle special cases where we have nested Component Blocks
  and Component Blocks inside a RepeatingGroup.
*/

#pragma once

namespace Fixpp
{

    namespace details
    {

        namespace flatten
        {

            namespace tuple
            {

                template<typename T1, typename T2> struct TupleCat;

                template<typename... Args1, typename... Args2>
                struct TupleCat<meta::tuple<Args1...>, meta::tuple<Args2...>>
                {
                    using Result = meta::tuple<Args1..., Args2...>;
                };

                template<template<typename> class FieldT, typename... Tags>
                struct Flatten;

                template<template<typename> class FieldT, typename T>
                struct FlattenSingle
                {
                    using Result = meta::tuple<FieldT<T>>;
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
                    using Result = meta::tuple<>;
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

            namespace pack
            {

                template<typename... Args>
                struct Flatten;

                template<typename T>
                struct FlattenSingle
                {
                    using Result = meta::pack::Pack<T>;
                };

                template<typename Head, typename... Tail>
                struct FlattenSingle<ComponentBlock<Head, Tail...>>
                {
                    using Result = typename meta::pack::ops::Append<
                                        typename FlattenSingle<Head>::Result,
                                        typename Flatten<Tail...>::Result
                                   >::Result;
                };

                template<typename Head, typename... Tail>
                struct Flatten<Head, Tail...>
                {
                    using Result = typename meta::pack::ops::Append<
                                        typename FlattenSingle<Head>::Result,
                                        typename Flatten<Tail...>::Result
                                   >::Result;
                };

                template<>
                struct Flatten<>
                {
                    using Result = meta::pack::Pack<>;
                };

            } // namespace pack

        } // namespace flatten

        template<template<typename> class FieldT, typename... Tags>
        struct Flattened
        {
            using Fields = typename flatten::tuple::Flatten<FieldT, Tags...>::Result;
            using List = typename flatten::typelist::Flatten<Tags...>::Result;
        };

    } // namespace details

} // namespace Fixpp
