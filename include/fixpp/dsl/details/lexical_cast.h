#pragma once

#include <cmath>

#include <fixpp/tag.h>
#include <fixpp/utils/cursor.h>
#include <fixpp/utils/time.h>

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
            static int64_t cast(const char* offset, size_t /*size*/)
            {
                int64_t x = 0;
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

        template<>
        struct LexicalCast<Type::UTCTimestamp>
        {
            static Type::UTCTimestamp::Time cast(const char* offset, size_t size)
            {
                RawStreamBuf<> buf(const_cast<char *>(offset), size);
                StreamCursor cursor(&buf);

                auto parseFragment = [&](size_t size, const char* error)
                {
                    int value;
                    if (!match_int_fast_n(&value, cursor, size))
                        throw std::runtime_error(error);

                    return value;
                };

                int year = parseFragment(4, "Could not parse year from UTCTimestamp");
                int month = parseFragment(2, "Could not parse month from UTCTimestamp");
                int day = parseFragment(2, "Could not parse month from UTCTimestamp");


                // Literal('-')
                if (!cursor.advance(1))
                    throw std::runtime_error("Could not parse hour from UTCTimestamp, expected '-' got EOF");

                int hour = parseFragment(2, "Could not parse hours from UTCTimestamp");
                //
                // Literal(':')
                if (!cursor.advance(1))
                    throw std::runtime_error("Could not parse minutes from UTCTimestamp, expected ':' got EOF");

                int min = parseFragment(2, "Could not parse minutes from UTCTimestamp");
                //
                // Literal(':')
                if (!cursor.advance(1))
                    throw std::runtime_error("Could not parse seconds from UTCTimestamp, expected ':' got EOF");

                int sec = parseFragment(2, "Could not parse seconds from UTCTimestamp");

                int msec = 0;

                if (!cursor.eof())
                {
                    // Literal('.')
                    cursor.advance(1);
                    msec = parseFragment(3, "Could not parse milliseconds from UTCTimestamp");
                }

                std::tm tm;
                // tm_year is year since 1900
                tm.tm_year = year - 1900;
                // tm_mon is 0-indexed
                tm.tm_mon = month - 1;
                tm.tm_mday = day;
                tm.tm_hour = hour;
                tm.tm_min = min;
                tm.tm_sec = sec;

                return Type::UTCTimestamp::Time(tm, msec, mkgmtime(&tm));
            }
        };

    } // namespace details;

} // namespace Fix
