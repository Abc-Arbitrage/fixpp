/* cursor.h
   Mathieu Stefani, 15 november 2016
   
*/

#pragma once

#include <cstddef>
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

    bool advance(size_t count);
    operator size_t() const { return buf->position(); }

    bool eof() const;
    int next() const;
    char current() const;

    const char* offset() const;
    const char* offset(size_t off) const;

    size_t diff(size_t other) const;
    size_t diff(const StreamCursor& other) const;

    size_t remaining() const;

    void reset();

public:
    StreamBuf<char>* buf;
};


enum class CaseSensitivity {
    Sensitive, Insensitive
};

bool match_raw(const void* buf, size_t len, StreamCursor& cursor);
bool match_string(const char *str, size_t len, StreamCursor& cursor,
        CaseSensitivity cs = CaseSensitivity::Insensitive);
bool match_literal(char c, StreamCursor& cursor, CaseSensitivity cs = CaseSensitivity::Insensitive);
bool match_literal_fast(char c, StreamCursor& cursor);
bool match_until(char c, StreamCursor& cursor, CaseSensitivity cs = CaseSensitivity::Insensitive);
bool match_until_fast(char c, StreamCursor& cursor);
bool match_until(std::initializer_list<char> chars, StreamCursor& cursor, CaseSensitivity cs = CaseSensitivity::Insensitive);
bool match_double(double* val, StreamCursor& cursor);
bool match_int(int* val, StreamCursor& cursor);
bool match_int_fast(int* val, StreamCursor& cursor);

void skip_whitespaces(StreamCursor& cursor);
