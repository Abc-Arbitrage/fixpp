/* chars.h
   Nicolas Fauvet, 08 may 2017
   
   A parameter pack of chars
*/

#pragma once

namespace Fixpp
{

    // ------------------------------------------------
    // Chars
    // ------------------------------------------------
    //

    template<char... Cs> struct Chars;

    namespace char_traits
    {
        template<typename Chars> struct Compare;

        template<char C1> struct Compare<Chars<C1>>
        {
            static bool eq(const char* value)
            {
                return value[0] == C1;
            }
        };

        template<char C1, char C2> struct Compare<Chars<C1, C2>>
        {
            static bool eq(const char* value)
            {
                return value[0] == C1 && value[1] == C2;
            }
        };
    };

    template<char... Cs> struct Chars
    {

        static constexpr size_t Size = sizeof...(Cs);
        static_assert(Size <= 2, "Too many characters");

        static constexpr const char Value[] = { Cs... };

        static bool equals(const char* msgType, size_t len)
        {
            if (len != Size)
                return false;

            return char_traits::Compare<Chars<Cs...>>::eq(msgType);
        }
    };

    template<char... Cs>
    constexpr const char Chars<Cs...>::Value[];

    template<char... Cs>
    constexpr size_t Chars<Cs...>::Size;

    template<> struct Chars<>
    {
    };

    using Empty = Chars<>;

} // namespace Fixpp
