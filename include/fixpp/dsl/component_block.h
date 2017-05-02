/* message.h
   Mathieu Stefani, 04 january 2017
   
  A phantom type for representing a FIX Component Block
*/

#pragma once

#include <fixpp/meta.h>

namespace Fixpp
{

    template<typename... Tags>
    struct ComponentBlock
    {
    };

} // namespace Fixpp
