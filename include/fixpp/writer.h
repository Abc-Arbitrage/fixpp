/* writer.h
   Mathieu Stefani, 12 november 2016
   
   A Writer for a FIX Message
*/

#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <fixpp/tag.h>
#include <fixpp/message.h>
#include <fixpp/streambuf.h>

namespace Fix {

static constexpr const char SOH = '|';

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

    template<typename GroupTag, typename... Tags>
    struct FieldWriter<Field<RepeatingGroup<GroupTag, Tags...>>>
    {
        template<typename FieldT>
        static std::ostream& write(std::ostream& os, const FieldT& field)
        {
            os << GroupTag::Id << "=" << field.size() << SOH;

            static constexpr size_t GroupSize = sizeof...(Tags);

            auto group = field.get();
            for (const auto& instance: group)
            {
                doWriteGroupFields(os, instance, meta::make_index_sequence<GroupSize>{});
            }

            return os;
        }

    private:
        template<typename Tuple, size_t... Index>
        static void doWriteGroupFields(std::ostream& os, const Tuple& tuple, meta::index_sequence<Index...>)
        {
            int dummy[] = { 0, ((void) doWriteField(os, std::get<Index>(tuple)), 0)... };
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

template<typename Field>
std::ostream& writeField(std::ostream& os, const Field& field)
{
    details::FieldWriter<Field>::write(os, field);
}

struct Writer
{
    template<typename Header, typename Message>
    std::string write(Header header, const Message& message)
    {
        StreamBuf buf;
        std::ostream os(&buf);

        if (!message.bits.all())
        {
            std::ostringstream error;
            const size_t missingBits = message.bits.size() - message.bits.count();
            error << "Missing " << missingBits << " required value(s) for Message";
            throw std::runtime_error(error.str());
        }

        set<Tag::MsgType>(header, Message::MsgType);
        set<Tag::SendingTime>(header, std::time(nullptr));
        set<Tag::MsgSeqNum>(header, 1);

        if (!header.bits.all())
        {
            throw std::runtime_error("Missing values for header");
        }

        write(os, header.values, meta::make_index_sequence<Header::TotalTags>());
        write(os, message.values, meta::make_index_sequence<Message::TotalTags>());

        const size_t size = buf.count();

        auto context = buf.save();

        buf.pubseekpos(0);

        writeTag<Tag::BeginString>(os, v42::Version::Str);
        writeTag<Tag::BodyLength>(os, size);

        buf.restore(context);

        const size_t checksum = buf.checksum();
        os << "10=" << std::setfill('0') << std::setw(3) << checksum;

        os << SOH;

        return buf.asString();
    }

    template<typename Tuple, size_t... Idx>
    void write(std::ostream& os, const Tuple& tuple, meta::index_sequence<Idx...>)
    {
        int dummy[] = {0, ((void) doWriteField(os, std::get<Idx>(tuple)), 0)...};
        (void) dummy;
    }

    template<typename Field>
    void doWriteField(std::ostream& os, const Field& field)
    {
        if (!field.empty())
            writeField<Field>(os, field);
    }
};

} // namespace Fix