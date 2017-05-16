/* tag.h
   Mathieu Stefani, 12 november 2016
   
*/
#pragma once

#include <streambuf>
#include <vector>
#include <array>

#pragma once

template<size_t N, typename CharT = char>
class SmallStreamBuf : public std::basic_streambuf<CharT>
{
public:
    using Base = std::basic_streambuf<CharT>;

    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;

    SmallStreamBuf()
        : short_buffer { }
    {
        Base::setp(short_buffer.data(), short_buffer.data() + short_buffer.size());
        buffer_ptr = short_buffer.data();
    }

    const CharT* buffer() const
    {
        return buffer_ptr;
    }

    bool isSmall() const
    {
        return long_buffer.empty();
    }

    SmallStreamBuf(const SmallStreamBuf& other) = delete;
    SmallStreamBuf& operator=(const SmallStreamBuf& other) = delete;

protected:
    int_type overflow(int_type ch) override
    {
        if (!traits_type::eq_int_type(ch, traits_type::eof()))
        {
            size_t oldSize = 0;
            size_t newSize = 0;

            if (long_buffer.empty())
            {
                oldSize = N;
                newSize = oldSize * 2;
                long_buffer.resize(newSize);
                std::copy(std::begin(short_buffer), std::end(short_buffer), std::begin(long_buffer));

                buffer_ptr = long_buffer.data();
            }
            else
            {
                oldSize = long_buffer.size();
                newSize = oldSize * 2;
                long_buffer.resize(newSize);
            }

            Base::setp(&long_buffer[0] + oldSize, &long_buffer[0] + newSize);
            *this->pptr() = static_cast<char>(ch);
            Base::pbump(1);

            return traits_type::not_eof(ch);
        }

        return traits_type::eof();
    }


private:
    std::array<CharT, N> short_buffer;
    std::vector<CharT> long_buffer;

    CharT* buffer_ptr;
};


namespace Fixpp
{

    template<typename Message, size_t N>
    struct StreamBufBase : public SmallStreamBuf<N>
    {
        using Base = SmallStreamBuf<N>;
        using char_type = typename Base::char_type;
        using pos_type = typename Base::pos_type;

        /*
            While sizeof on a string literal of type const char[N] will also include the NUL termination
            character, we don't need to substract it as we have to insert an SOH after every-tag.
            Thus, it's as if SOH was a NUL
        */
        static constexpr size_t BeginStringSize = sizeof("8=") + Message::Version::Size;
        static constexpr size_t BodyLengthSize = sizeof("9=00000");

        static constexpr size_t MaxBodyLengthDigits = 5;

        static constexpr size_t HeaderStartLength = BeginStringSize + BodyLengthSize;

        struct Context
        {
            char *pbase;
            char *pptr;
            char *epptr;
        };

        StreamBufBase()
            : off_(0)
        {
            Base::pbump(HeaderStartLength);
        }

        std::streamsize offset() const
        {
            return static_cast<std::streamsize>(Base::pptr() - Base::pbase());
        }

        void prepareWriteHeader(size_t bodySize)
        {
            const auto bodySizeDigits = digitsCount(bodySize);
            off_ = MaxBodyLengthDigits - bodySizeDigits;
            Base::pubseekpos(off_);
        }

        size_t checksum() const
        {
            const char* p = Base::pbase() + off_;
            const char* end = Base::pptr();

            size_t sum = 0;

            while (p != end)
            {
                sum += static_cast<size_t>(*p);
                ++p;
            }

            return sum % 256;
        }

        Context save() const
        {
            return Context { Base::pbase(), Base::pptr(), Base::epptr() };
        }

        void restore(const Context& context)
        {
            Base::setp(context.pbase, context.epptr);
            Base::pbump(static_cast<int>(context.pptr - context.pbase));
        }

        std::string asString() const
        {
            return std::string(Base::pbase() + off_, Base::pptr());
        }

    private:
        size_t off_;

        int digitsCount(size_t x)
        {  
            return (x < 10 ? 1 :   
                (x < 100 ? 2 :   
                (x < 1000 ? 3 :   
                (x < 10000 ? 4 :   
                (x < 100000 ? 5 :   
                (x < 1000000 ? 6 :   
                (x < 10000000 ? 7 :  
                (x < 100000000 ? 8 :  
                (x < 1000000000 ? 9 :  
                10)))))))));  
        }

    protected:
        pos_type seekpos(pos_type pos, std::ios_base::openmode) override
        {
            auto* base = Base::pbase();
            auto* end = Base::epptr();

            Base::setp(base + pos, end); 
            return pos;
        }
    };

    namespace internal
    {
        static constexpr size_t SmallBufferSize = 1024;
    } // namespace internal

    template<typename Message>
    using StreamBuf = StreamBufBase<Message, internal::SmallBufferSize>;

} // namespace Fixpp
