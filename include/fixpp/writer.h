/* writer.h
   Mathieu Stefani, 12 november 2016
   
   A Writer for a FIX Message
*/

#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <fixpp/tag.h>
#include <fixpp/dsl.h>
#include <fixpp/streambuf.h>
#include <fixpp/utils/soh.h>

namespace Fixpp {

namespace details
{
    template<typename Field>
    struct FieldWriter
    {
        static std::ostream& write(std::ostream& os, const Field& field)
        {
            auto value = field.get();
            os << Field::Tag::Id << "=" << value << SOH;
            return os;
        }
    };

    template<typename GroupTag, typename SizeHint, typename... Tags>
    struct FieldWriter<Field<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>>
    {
        template<typename FieldT>
        static std::ostream& write(std::ostream& os, const FieldT& field)
        {
            os << GroupTag::Id << "=" << field.size() << SOH;

            static constexpr size_t GroupSize = FieldT::TotalTags;

            const auto& group = field.get();
            for (const auto& instance: group)
            {
                doWriteGroupFields(os, instance, meta::seq::make_index_sequence<GroupSize>{});
            }

            return os;
        }

    private:
        template<typename Tuple, size_t... Index>
        static void doWriteGroupFields(std::ostream& os, const Tuple& tuple, meta::seq::index_sequence<Index...>)
        {
            int dummy[] = { 0, ((void) doWriteField(os, meta::get<Index>(tuple.values)), 0)... };
            (void) dummy;
        }

        template<typename Field>
        static void doWriteField(std::ostream& os, const Field& field)
        {
            if (!field.empty())
                FieldWriter<Field>::write(os, field);
        }

    };


} // namespace details

template<typename Tag, typename Value>
std::ostream& writeTag(std::ostream& os, Value&& value)
{
    static_assert(details::IsValidTypeFor<Tag, Value>::value,
                  "Invalid data type for given Tag");

    os << Tag::Id << "=" << value << SOH;
    return os;
}

template<typename Tag>
std::ostream& writeRaw(std::ostream& os, const char* data, size_t len)
{
    os << Tag::Id << "=";
    os.write(data, len);
    os << SOH;

    return os;
}

template<typename Field>
std::ostream& writeField(std::ostream& os, const Field& field)
{
    return details::FieldWriter<Field>::write(os, field);
}

struct Writer
{
    template<typename Header, typename Message>
    std::string write(const Header& header, const Message& message)
    {
        StreamBuf<Message> buf;
        std::ostream os(&buf);

        if (!message.requiredBits.all())
        {
            std::ostringstream error;
            const size_t missingBits = message.requiredBits.size() - message.requiredBits.count();
            error << "Missing " << missingBits << " required value(s) for Message";
            throw std::runtime_error(error.str());
        }

        if (!header.requiredBits.all())
        {
            throw std::runtime_error("Missing values for header");
        }

        const auto startOffset = buf.offset();

        writeRaw<Tag::MsgType>(os, Message::MsgType::Value, Message::MsgType::Size);
        write(os, header.values, meta::seq::make_index_sequence<Header::TotalTags>());
        write(os, message.values, meta::seq::make_index_sequence<Message::TotalTags>());

        const auto endOffset = buf.offset();        

        const size_t size = endOffset - startOffset;

        auto context = buf.save();

        buf.prepareWriteHeader(size);

        writeTag<Tag::BeginString>(os, Message::Version::Str);
        os << Tag::BodyLength::Id << "=" << size << SOH;

        buf.restore(context);

        const size_t checksum = buf.checksum();
        os << "10=" << std::setfill('0') << std::setw(3) << checksum;

        os << SOH;

        return buf.asString();
    }

    template<typename Tuple, size_t... Idx>
    void write(std::ostream& os, const Tuple& tuple, meta::seq::index_sequence<Idx...>)
    {
        int dummy[] = {0, ((void) doWriteField(os, meta::get<Idx>(tuple)), 0)...};
        (void) dummy;
    }

    template<typename Field>
    void doWriteField(std::ostream& os, const Field& field)
    {
        if (!field.empty())
            writeField<Field>(os, field);
    }
};

} // namespace Fixpp
