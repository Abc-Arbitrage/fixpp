/* view.h
   Mathieu Stefani, 11 may 2017
   
  A read-only view on a buffer
*/

#pragma once

#include <utility>

namespace Fixpp
{

    // @Todo use string_view when available
    using View = std::pair<const char*, size_t>;

    inline View make_view(const char* data, size_t size)
    {
        return std::make_pair(data, size);
    }
}
