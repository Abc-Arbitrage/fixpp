#pragma once

#include <fixpp/dsl/required.h>

namespace Fixpp
{
    namespace details
    {

        template<typename T> struct Unwrap
        {
            using Result = T;
        };

        template<typename T> struct Unwrap<Required<T>>
        {
            using Result = T;
        };

    } // namespace details

} // namespace Fixpp
