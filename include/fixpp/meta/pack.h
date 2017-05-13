/* map.h
   Mathieu Stefani, 13 may 2017
   
  A compile-time parameter pack
*/

#pragma once

namespace meta
{
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

		template<typename T, T... Vals>
        struct ValuePack
        {
        };

    } // namespace pack
} // namespace meta
