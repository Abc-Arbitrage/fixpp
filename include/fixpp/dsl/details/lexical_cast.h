#pragma once

#include <cmath>

namespace Fixpp
{
    namespace details
    {
        template<typename T> struct LexicalCast
        {
            static typename T::UnderlyingType cast(const char*, size_t)
            {
                return {};
            }
        };

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
            static int cast(const char* offset, size_t /*size*/)
            {
                int x = 0;
                bool neg = false;
                if (*offset == '-') {
                    neg = true;
                    ++offset;
                }
                while (*offset >= '0' && *offset <= '9') {
                    x = (x*10) + (*offset - '0');
                    ++offset;
                }
                if (neg) {
                    x = -x;
                }
                return x;
            }
        };

        template<>
        struct LexicalCast<Type::Float>
        {
            static double cast(const char* offset, size_t /*size*/)
            {
                double r = 0.0;
                bool neg = false;
                if (*offset == '-') {
                    neg = true;
                    ++offset;
                }
                while (*offset >= '0' && *offset <= '9') {
                    r = (r*10.0) + (*offset - '0');
                    ++offset;
                }
                if (*offset == '.') {
                    double f = 0.0;
                    int n = 0;
                    ++offset;
                    while (*offset >= '0' && *offset <= '9') {
                        f = (f*10.0) + (*offset - '0');
                        ++offset;
                        ++n;
                    }
                    r += f / std::pow(10.0, n);
                }
                if (neg) {
                    r = -r;
                }
                return r;
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
