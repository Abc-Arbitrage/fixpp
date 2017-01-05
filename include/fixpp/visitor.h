/* visitor.h
   Mathieu Stefani, 15 november 2016
   
  A typed visitor for a FIX frame
*/

#pragma once

#include <cstring>
#include <type_traits>

#include <fixpp/tag.h>
#include <fixpp/utils/cursor.h>
#include <fixpp/meta.h>
#include <fixpp/dsl/details/unwrap.h>

namespace Fix
{
    namespace impl
    {
        namespace rules
        {
            template<typename... > using void_t = void;

            template<typename T, typename = void> struct HasOverrides : std::false_type { };
            template<typename T>
            struct HasOverrides<
                    T,
                    void_t<typename T::Overrides>
                  > : std::true_type
            { };

            template<typename T, typename = void> struct HasValidateChecksum : std::false_type { };
            template<typename T>
            struct HasValidateChecksum<
                    T,
                    void_t<decltype(&T::ValidateChecksum)>
                  > : std::true_type
            { };

            template<typename T, typename = void> struct HasValidateLength : std::false_type { };
            template<typename T>
            struct HasValidateLength<
                    T,
                    void_t<decltype(&T::ValidateLength)>
                  > : std::true_type
            { };

            template<typename Overrides>
            struct OverridesValidator;

            template<typename First, typename Second, typename... Rest>
            struct OverridesValidator<meta::map::Map<meta::map::Pair<First, Second>, Rest...>> : public OverridesValidator<meta::map::Map<Rest...>>
            {
                static_assert(First::MsgType == Second::MsgType, "Invalid Override: Message types must be the same");
            };

            template<>
            struct OverridesValidator<meta::map::Map<>>
            {
            };

        } // namespace rules


        template<typename T> struct id { };

        template<typename Message, typename Overrides> using OverrideFor
            = typename meta::map::ops::atOr<Overrides, Message, Message>::type::Ref;

        template<typename Visitor, typename Rules>
        void visitMessageType(char msgType, const char* version, size_t size, Visitor visitor, Rules)
        {
            using Overrides = typename Rules::Overrides;

            using Version42 = Fix::v42::Version;
            if (Version42::equals(version))
            {
                using Header = Fix::v42::Header::Ref;

                switch (msgType)
                {
                    case '0':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::Heartbeat, Overrides>> {});
                    case '1':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::TestRequest, Overrides>> {});
                        break;
                    case '2':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::ResendRequest, Overrides>> {});
                        break;
                    case '3':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::Reject, Overrides>> {});
                        break;
                    case '4':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::SequenceReset, Overrides>> {});
                        break;
                    case '5':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::Logout, Overrides>> {});
                        break;
                    case '6':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::IndicationOfInterest, Overrides>> {});
                        break;
                    case 'A':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::Logon, Overrides>> {});
                        break;
                    case 'V':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::MarketDataRequest, Overrides>> {});
                        break;
                    case 'W':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::MarketDataSnapshot, Overrides>> {});
                        break;
                    case 'X':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::MarketDataIncrementalRefresh, Overrides>> {});
                }
            }

            using Version44 = Fix::v44::Version;
            if (Version44::equals(version))
            {
                using Header = Fix::v44::Header::Ref;

                switch (msgType)
                {
                    case 'W':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v44::Message::MarketDataSnapshot, Overrides>> {});
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

        template<typename... > struct Pack
        {
        };

        template<typename Pack, typename Value>
        struct Append;

        template<typename... Vals, typename Value>
        struct Append<Pack<Vals...>, Value>
        {
            using Result = Pack<Vals..., Value>;
        };

        template<typename... Vals, typename... Others>
        struct Append<Pack<Vals...>, Pack<Others...>>
        {
            using Result = Pack<Vals..., Others...>;
        };

        namespace details
        {
            namespace pack
            {
                
                template<typename... Args>
                struct Flatten;

                template<typename T>
                struct FlattenSingle
                {
                    using Result = Pack<T>;
                };

                template<typename Head, typename... Tail>
                struct FlattenSingle<ComponentBlock<Head, Tail...>>
                {
                    using Result = typename Append<
                                        typename FlattenSingle<Head>::Result,
                                        typename Flatten<Tail...>::Result
                                   >::Result;
                };

                template<typename Head, typename... Tail>
                struct Flatten<Head, Tail...>
                {
                    using Result = typename Append<
                                        typename FlattenSingle<Head>::Result,
                                        typename Flatten<Tail...>::Result
                                   >::Result;
                };

                template<>
                struct Flatten<>
                {
                    using Result = Pack<>;
                };

            }
        }

        template<typename Field>
        struct FieldGroupVisitor
        {
            void operator()(Field& field, const std::pair<const char*, size_t>& view)
            {
                field.set(view);
            }
        };

        template<typename GroupTag, typename... Tags>
        struct FieldGroupVisitor<FieldRef<RepeatingGroup<GroupTag, Tags...>>>
        {
            void operator()(FieldRef<RepeatingGroup<GroupTag, Tags...>>& field, const std::pair<const char*, size_t>& view)
            {
            }
        };

        namespace details
        {
            template<typename Tag>
            struct UnwrapTag
            {
                static constexpr int Id = Tag::Id;
            };

            template<typename Tag>
            struct UnwrapTag<Required<Tag>>
            {
                static constexpr int Id = Tag::Id;
            };

            template<typename GroupTag, typename... Tags>
            struct UnwrapTag<RepeatingGroup<GroupTag, Tags...>>
            {
                static constexpr int Id = GroupTag::Id;
            };

            template<typename Pack> struct IndexesImpl;

            template<typename... Tags>
            struct IndexesImpl<Pack<Tags...>>
            {
                static constexpr std::array<int, sizeof...(Tags)> Value = {
                    UnwrapTag<Tags>::Id...
                };
            };

            template<typename... Tags>
            constexpr std::array<int, sizeof...(Tags)>
            IndexesImpl<Pack<Tags...>>::Value;

            template<typename... Tags>
            struct Indexes : public IndexesImpl<typename pack::Flatten<Tags...>::Result>
            {
            };
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
                    using Indexes = details::Indexes<Tags...>;

                    auto it = std::find(std::begin(Indexes::Value), std::end(Indexes::Value), tag);
                    if (it == std::end(Indexes::Value))
                    {
                        return -1;
                    }

                    return std::distance(std::begin(Indexes::Value), it);
                    //return 0;
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
                    FieldGroupVisitor<Field> visitor;
                    visitor(field, view);
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

        template<typename Visitor, typename Overrides>
        void visitMessage(char msgType, const char* version, size_t size, StreamCursor& cursor, Visitor visitor, Overrides overrides)
        {
            MessageVisitor<Visitor> messageVisitor(cursor);
            visitMessageType(msgType, version, size, messageVisitor, overrides);
        }

    } // namespace impl

    struct VisitRules
    {
        template<typename... Args>
        using OverrideSet = meta::map::Map<Args...>;

        template<typename First, typename Second>
        using Override = meta::map::Pair<First, Second>;

        template<typename T> using As = T;
    };

    struct DefaultRules : public VisitRules
    {
        using Overrides = OverrideSet<>;

        static constexpr bool ValidateChecksum = true;
        static constexpr bool ValidateLength = true;
    };

    template<typename Visitor, typename Rules>
    void visit(const char* frame, size_t size, Visitor visitor, Rules rules)
    {
        static_assert(std::is_base_of<VisitRules, Rules>::value, "Visit rules must inherit from VisitRules");

        static_assert(impl::rules::HasOverrides<Rules>::value, "Visit rules must provide an Overrides typedef");
        static_assert(impl::rules::HasValidateChecksum<Rules>::value, "Visit rules must provide a static ValidateChecksum boolean");
        static_assert(impl::rules::HasValidateLength<Rules>::value, "Visit rules must provide a static ValidateLength boolean");

        impl::rules::OverridesValidator<typename Rules::Overrides> validator {};

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

        impl::visitMessage(msgType.second, beginString.second.first, beginString.second.second, cursor, visitor, rules);
    }

    template<typename Visitor>
    void visit(const char* frame, size_t size, Visitor visitor)
    {
        visit(frame, size, visitor, DefaultRules {});
    }

} // namespace Fix
