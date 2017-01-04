#pragma once

#include <cstring>

namespace Fix
{
    namespace details
    {
        template<typename T> struct LexicalCast;

        template<>
        struct LexicalCast<Type::Boolean>
        {
            static bool cast(const char* offset, size_t)
            {
                return *offset == 'Y';
            }
        };

        template<>
        struct LexicalCast<Type::Char>
        {
            static char cast(const char* offset, size_t)
            {
                if (offset)
                    return *offset;

                return 0;
            }
        };

        template<>
        struct LexicalCast<Type::Int>
        {
            static int cast(const char* offset, size_t size)
            {
                char *end;
                return strtol(offset, &end, 10);
            }
        };

        template<>
        struct LexicalCast<Type::String>
        {
            static std::string cast(const char* offset, size_t size)
            {
                return std::string(offset, size);
            }
        };

    } // namespace details;

} // namespace Fix
