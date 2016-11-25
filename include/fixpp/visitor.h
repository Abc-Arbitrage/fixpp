/* visitor.h
   Mathieu Stefani, 15 november 2016
   
  A typed visitor for a FIX frame
*/

#pragma once

#include <cstring>

#include <fixpp/tag.h>
#include <fixpp/utils/cursor.h>
#include <fixpp/meta.h>

namespace Fix
{
    namespace impl
    {
        template<typename T> struct id { };

        template<typename Visitor>
        void visitMessageType(char msgType, const char* version, size_t size, Visitor visitor)
        {
            using Version42 = Fix::v42::Version;
            if (Version42::equals(version))
            {
                using Header = Fix::v42::Header::Ref;

                switch (msgType)
                {
                    case '0':
                        visitor(id<Header> {}, id<Fix::v42::Message::Heartbeat::Ref> {});
                        break;
                    case 'A':
                        visitor(id<Header> {}, id<Fix::v42::Message::Logon::Ref> {});
                        break;
                }
            }
        }

        template<typename Field, typename Visitor>
        bool doVisitSingleField(unsigned tag, Field& field, Visitor visitor)
        {
            if (tag == field.tag())
            {
                visitor(field);
                return true;
            }
            return false;
        }

        template<typename Message, typename Visitor, size_t... Index>
        bool doVisitField(Message& message, unsigned tag, Visitor visitor,
                          meta::index_sequence<Index...>)
        {
            bool matches[] = {false, (doVisitSingleField(tag, std::get<Index>(message.values), visitor))...};
            return std::any_of(std::begin(matches), std::end(matches), [](bool b) { return b; });
        }

        template<typename Message, typename Visitor>
        bool visitField(Message& message, unsigned tag, Visitor visitor)
        {
            static constexpr size_t Size = Message::TotalTags;
            return doVisitField(message, tag, visitor, meta::make_index_sequence<Size>());
        }

        template<typename T>
        struct TagMatcher;

        template<>
        struct TagMatcher<Type::Char>
        {
            using Return = char;

            static bool matchValue(Return* value, StreamCursor& cursor)
            {
                *value = cursor.current();
                cursor.advance(1);
                return true;
            }
        };

        template<>
        struct TagMatcher<Type::String>
        {
            using Return = std::pair<const char*, size_t>;

            static bool matchValue(Return* value, StreamCursor& cursor)
            {
                StreamCursor::Token valueToken(cursor);
                if (!match_until('|', cursor)) return false;

                *value = valueToken.view(); 
                return true;
            }
        };

        template<>
        struct TagMatcher<Type::Int>
        {
            using Return = int;

            static bool matchValue(Return* value, StreamCursor& cursor)
            {
                int val;
                if (!match_int(&val, cursor)) return false;

                *value = val;
                return true;
            }
        };

        template<
            typename Tag,
            typename Ret = typename TagMatcher<typename Tag::Type>::Return>
        std::pair<bool, Ret> matchTag(StreamCursor& cursor)
        {
            #define TRY(...) \
            if (!__VA_ARGS__) \
               return std::make_pair(false, Ret {})

            StreamCursor::Revert revert(cursor);
            
            int tag;
            TRY(match_int(&tag, cursor));
            
            if (tag != Tag::Id)
               return std::make_pair(nullptr, Ret {});

            TRY(match_literal('=', cursor));

            Ret ret;

            TRY(TagMatcher<typename Tag::Type>::matchValue(&ret, cursor));

            revert.ignore();

            return std::make_pair(true, ret);

            #undef TRY
        }

        template<typename Field> struct FieldParser;

        template<typename TagT>
        struct FieldParser<FieldRef<TagT>>
        {
            using Field = FieldRef<TagT>;

            void parse(Field& field, StreamCursor& cursor)
            {
                StreamCursor::Token valueToken(cursor);
                match_until('|', cursor);

                auto view = valueToken.view();
                cursor.advance(1);

                field.set(view);
            }
        };

        template<typename GroupTag, typename... Tags>
        struct FieldParser<FieldRef<RepeatingGroup<GroupTag, Tags...>>>
        {
            using Field = FieldRef<RepeatingGroup<GroupTag, Tags...>>;

            struct GroupSet
            {
                void set(unsigned tag)
                {
                    bits.set(tagIndex(tag));
                }

                bool isset(unsigned tag)
                {
                    return bits.test(tagIndex(tag));
                }

                void reset()
                {
                    bits.reset();
                }

                bool valid(unsigned tag)
                {
                    return tagIndex(tag) != -1;
                }

            private:
                int64_t tagIndex(unsigned tag) const
                {
                    static constexpr std::array<int, sizeof...(Tags)> TagIndexes = {
                        Tags::Id...
                    };

                    auto it = std::find(std::begin(TagIndexes), std::end(TagIndexes), tag);
                    if (it == std::end(TagIndexes))
                    {
                        return -1;
                    }

                    return std::distance(std::begin(TagIndexes), it);
                }

                std::bitset<sizeof...(Tags)> bits;
            };

            struct Visitor
            {
                Visitor(const std::pair<const char*, size_t>& view)
                    : view(view)
                { }

                template<typename Field>
                void operator()(Field& field)
                {
                    field.set(view);
                }

            private:
                std::pair<const char*, size_t> view;
            };


            void parse(Field& field, StreamCursor& cursor)
            {
                int instances;
                match_int(&instances, cursor);
                cursor.advance(1);

                field.reserve(instances);

                GroupSet groupSet;
                int tag;

                bool inGroup = true;

                do
                {
                    typename Field::RefType groupRef;

                    for (;;)
                    {

                        StreamCursor::Revert revertTag(cursor);
                        match_int(&tag, cursor);

                        if (!groupSet.valid(tag))
                        {
                            inGroup = false;
                            break;
                        }

                        if (groupSet.isset(tag))
                            break;

                        revertTag.ignore();
                        cursor.advance(1);

                        groupSet.set(tag);

                        StreamCursor::Token valueToken(cursor);
                        match_until('|', cursor);

                        auto view = valueToken.view();
                        Visitor visitor(view);
                        visitField(groupRef, tag, visitor);

                        cursor.advance(1);

                    }

                    field.add(std::move(groupRef));
                    groupSet.reset();

                } while (inGroup);

            }
        };

        struct FieldVisitor
        {
            FieldVisitor(StreamCursor& cursor)
                : cursor(cursor)
            { }

            template<typename Field>
            void operator()(Field& field)
            {
                FieldParser<Field> parser;
                parser.parse(field, cursor);
            }

        private:
            StreamCursor& cursor;
        };

        template<typename Visitor>
        struct MessageVisitor
        {
            MessageVisitor(StreamCursor& cursor)
                : cursor(cursor)
            { }

            template<typename Message, typename Header>
            void operator()(id<Header>, id<Message>)
            {
                Header header;
                Message message;

                while (!cursor.eof())
                {
                    int tag;
                    match_int(&tag, cursor);
                    cursor.advance(1);

                    FieldVisitor fieldVisitor(cursor);
                    if (visitField(message, tag, fieldVisitor))
                        continue;
                    if (visitField(header, tag, fieldVisitor))
                        continue;

                    std::cout << "tag " << tag << " does not belong to message" << std::endl;

                    StreamCursor::Token valueToken(cursor);
                    match_until('|', cursor);

                    std::cout << "Value = " << valueToken.text() << std::endl;
                    cursor.advance(1);

                }

                visitor(header, message);
            }

        private:
            Visitor visitor;
            StreamCursor& cursor;
        };

        template<typename Visitor>
        void visitMessage(char msgType, const char* version, size_t size, StreamCursor& cursor, Visitor visitor)
        {
            MessageVisitor<Visitor> messageVisitor(cursor);
            visitMessageType(msgType, version, size, messageVisitor);
        }
    }

    template<typename Visitor>
    void visit(const char* frame, size_t size, Visitor visitor)
    {
        RawStreamBuf<> streambuf(const_cast<char *>(frame), size);
        StreamCursor cursor(&streambuf);

        auto beginString = impl::matchTag<Tag::BeginString>(cursor);

        if (!cursor.advance(1))
            return;

        if (!impl::matchTag<Tag::BodyLength>(cursor).first)
            return;

        if (!cursor.advance(1))
            return;

        auto msgType = impl::matchTag<Tag::MsgType>(cursor);
        
        if (!cursor.advance(1))
            return;

        impl::visitMessage(msgType.second, beginString.second.first, beginString.second.second, cursor, visitor);
    }

} // namespace Fix
