/* required.h
   Mathieu Stefani, 04 january 2017
   
   Phantom type for a Required tag
*/

#pragma once

namespace Fixpp
{

    // ------------------------------------------------
    // Required
    // ------------------------------------------------

    // A phantom type that acts as a "marker" for a required
    // field inside a Message

    template<typename TagT> struct Required
    {
    };

} // namespace Fixpp
