/* map.h
   Mathieu Stefani, 13 may 2017
   
  A naive compile-time map
*/

#pragma once

namespace meta
{
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

} // namespace meta
