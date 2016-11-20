/* base.h
   Mathieu Stefani, 15 november 2016
   
  Base macro for FIX versions
*/

#pragma once

#include <cstring>

#define FIX_BEGIN_VERSION_NAMESPACE(name, string)                          \
    namespace name                                                         \
    {                                                                      \
        struct Version                                                     \
        {                                                                  \
            static constexpr const char* Str = string;                     \
            static constexpr size_t Size = sizeof(string) - 1;             \
            static bool equals(const char* version)                        \
            {                                                              \
                return !strncmp(Str, version, Size);                       \
            }                                                              \
        };                                                                 \
        template<char MsgTypeChar, typename... Tags>                       \
        using MessageV = VersionnedMessage<Version, MsgTypeChar, Tags...>; \
    }                                                                      \
    namespace name                                                         \

#define FIX_END_VERSION_NAMESPACE
