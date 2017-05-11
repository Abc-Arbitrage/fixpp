/* cursor.h
   Mathieu Stefani, 15 november 2016
   
*/

#pragma once

#include <cstddef>
#include <cctype>
#include <cstring>
#include <streambuf>

template<typename CharT = char>
class StreamBuf : public std::basic_streambuf<CharT> {
public:
    typedef std::basic_streambuf<CharT> Base;
    typedef typename Base::traits_type traits_type;

    void setArea(char* begin, char *current, char *end) {
        this->setg(begin, current, end);
    }

    CharT *begptr() const {
        return this->eback();
    }

    CharT* curptr() const {
        return this->gptr();
    }

    CharT* endptr() const {
        return this->egptr();
    }

    size_t position() const {
        return this->gptr() - this->eback();
    }

    void reset() {
        this->setg(nullptr, nullptr, nullptr);
    }

    typename Base::int_type snext() const {
        if (this->gptr() == this->egptr()) {
            return traits_type::eof();
        }

        const CharT* gptr = this->gptr();
        return *(gptr + 1);
    }

};

template<typename CharT = char>
class RawStreamBuf : public StreamBuf<CharT> {
public:
    typedef StreamBuf<CharT> Base;

    RawStreamBuf(char* begin, char* end) {
        Base::setg(begin, begin, end);
    }
    RawStreamBuf(char* begin, size_t len) {
        Base::setg(begin, begin, begin + len);
    }

};

class StreamCursor {
public:
    StreamCursor(StreamBuf<char>* buf, size_t initialPos = 0)
        : buf(buf)
    {
        advance(initialPos);
    }

    static constexpr int Eof = -1;

    struct Token {
        Token(StreamCursor& cursor)
            : cursor(cursor)
            , position(cursor.buf->position())
            , eback(cursor.buf->begptr())
            , gptr(cursor.buf->curptr())
            , egptr(cursor.buf->endptr())
        { }

        size_t start() const { return position; }

        size_t end() const {
            return cursor.buf->position();
        }

        size_t size() const {
            return end() - start();
        }

        std::string text() {
            return std::string(gptr, size());
        }

        const char* rawText() const {
            return gptr;
        }

        std::pair<const char*, size_t> view() const
        {
            return std::make_pair(gptr, size());
        }

    private:
        StreamCursor& cursor;
        size_t position;
        char *eback;
        char *gptr;
        char *egptr;
    };

    struct Revert {
        Revert(StreamCursor& cursor)
            : cursor(cursor)
            , eback(cursor.buf->begptr())
            , gptr(cursor.buf->curptr())
            , egptr(cursor.buf->endptr())
            , active(true)
        { }

        ~Revert() {
            if (active)
                revert();
        }

        void revert() {
            cursor.buf->setArea(eback, gptr, egptr);
        }

        void ignore() {
            active = false;
        }

    private:
        StreamCursor& cursor;
        char *eback;
        char *gptr;
        char *egptr;
        bool active;

    };

    bool advance(size_t count)
    {
        if (count > static_cast< size_t >(buf->in_avail()))
            return false;

        for (size_t i = 0; i < count; ++i) {
            buf->sbumpc();
        }

        return true;
    }

    operator size_t() const { return buf->position(); }

    bool eof() const
    {
        return remaining() == 0;
    }

    int next() const
    {
        if (buf->in_avail() < 1)
            return Eof;

        return buf->snext();
    }

    char current() const
    {
        return static_cast< char >(buf->sgetc());
    }

    const char* offset() const
    {
        return buf->curptr();
    }

    const char* offset(size_t off) const
    {
        return buf->begptr() + off;
    }

    size_t diff(size_t other) const
    {
        return buf->position() - other;
    }

    size_t diff(const StreamCursor& other) const
    {
        return other.buf->position() - buf->position();
    }

    size_t remaining() const
    {
        return buf->in_avail();
    }

    void reset()
    {
        buf->reset();
    }

public:
    StreamBuf<char>* buf;
};


enum class CaseSensitivity {
    Sensitive, Insensitive
};

inline bool match_raw(const void* buf, size_t len, StreamCursor& cursor)
{
    if (cursor.remaining() < len)
        return false;

    if (std::memcmp(cursor.offset(), buf, len) == 0) {
        cursor.advance(len);
        return true;
    }

    return false;
}

inline bool match_string(const char *str, size_t len, StreamCursor& cursor,
        CaseSensitivity cs = CaseSensitivity::Insensitive)
{
    if (cursor.remaining() < len)
        return false;

    if (cs == CaseSensitivity::Sensitive) {
        if (std::strncmp(cursor.offset(), str, len) == 0) {
            cursor.advance(len);
            return true;
        }
    } else {
        const char *off = cursor.offset();
        for (size_t i = 0; i < len; ++i) {
            const auto lhs = std::tolower(str[i]);
            const auto rhs = std::tolower(off[i]);
            if (lhs != rhs) return false;
        }

        cursor.advance(len);
        return true;
    }

    return false;
}

inline bool match_literal(char c, StreamCursor& cursor, CaseSensitivity cs = CaseSensitivity::Insensitive)
{
    if (cursor.eof())
        return false;

    char lhs = (cs == CaseSensitivity::Sensitive ? c : static_cast< char >(std::tolower(c)));
    char rhs = (cs == CaseSensitivity::Sensitive ? cursor.current() : static_cast< char >(std::tolower(cursor.current())));

    if (lhs == rhs) {
        cursor.advance(1);
        return true;
    }

    return false;
}

inline bool match_literal_fast(char c, StreamCursor& cursor)
{
    if (cursor.eof())
        return false;

    if (c == cursor.current()) {
        cursor.advance(1);
        return true;
    }

    return false;
}

inline bool match_until(char c, StreamCursor& cursor, CaseSensitivity cs = CaseSensitivity::Insensitive)
{
    return match_until( { c }, cursor, cs);
}

inline bool match_until_fast(char c, StreamCursor& cursor)
{
    while (!cursor.eof()) {
        const char cur = cursor.current();
        if (c == cur) return true;
        cursor.advance(1);
    }

    return false;
}

inline bool match_until(std::initializer_list<char> chars, StreamCursor& cursor, CaseSensitivity cs = CaseSensitivity::Insensitive)
{
    if (cursor.eof())
        return false;

    auto find = [&](char val) {
        for (auto c: chars) {
            char lhs = cs == CaseSensitivity::Sensitive ? c : static_cast< char >(std::tolower(c));
            char rhs = cs == CaseSensitivity::Insensitive ? val : static_cast< char >(std::tolower(val));

            if (lhs == rhs) return true;
        }

        return false;
    };

    while (!cursor.eof()) {
        const char c = cursor.current();
        if (find(c)) return true;
        cursor.advance(1);
    }

    return false;
}

inline bool match_double(double* val, StreamCursor& cursor)
{
    // @Todo: strtod does not support a length argument
    char *end;
    *val = strtod(cursor.offset(), &end);
    if (end == cursor.offset())
        return false;

    cursor.advance(static_cast<ptrdiff_t>(end - cursor.offset()));
    return true;
}

inline bool match_int(int* val, StreamCursor& cursor)
{
    char *end;
    *val = strtol(cursor.offset(), &end, 10);
    if (end == cursor.offset())
        return false;

    cursor.advance(static_cast<ptrdiff_t>(end - cursor.offset()));
    return true;
}

inline bool match_int_fast_n(int *val, StreamCursor& cursor, size_t size)
{
    if (cursor.eof())
        return false;

    size_t i = 0;
    const char* p = cursor.offset();
    size_t remaining = cursor.remaining();

    int integer = 0;

    for (;;)
    {
        const char c = *p++;
        if (!(c >= '0' && c <= '9'))
            break;

        integer *= 10;
        integer += (c - '0');

        ++i;
        if (--remaining == 0 || (size > 0 && i == size))
            break;
    }

    if (i > 0) {
        *val = integer;
        cursor.advance(i);
        return true;
    }

    return false;

}

inline bool match_int_fast(int* val, StreamCursor& cursor)
{
    return match_int_fast_n(val, cursor, 0);
}

inline void skip_whitespaces(StreamCursor& cursor)
{
    if (cursor.eof())
        return;

    int c;
    while ((c = cursor.current()) != StreamCursor::Eof && (c == ' ' || c == '\t')) {
        cursor.advance(1);
    }
}
