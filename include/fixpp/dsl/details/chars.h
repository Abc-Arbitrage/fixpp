/* chars.h
   Nicolas Fauvet, 08 may 2017
   
  A structure of compiled time chars
*/

#pragma once


namespace Fixpp
{

    // ------------------------------------------------
    // Chars
    // ------------------------------------------------

    // A parameter pack of char...

    template<char ...> struct Chars
    {
    };

    using Empty = Chars<>;

} // namespace Fixpp
