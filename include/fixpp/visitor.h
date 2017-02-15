/* visitor.h
   Mathieu Stefani, 15 november 2016
   
  A typed visitor for a FIX frame
*/

#pragma once

#include <cstring>
#include <type_traits>
#include <algorithm>

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>

#include <fixpp/tag.h>
#include <fixpp/utils/cursor.h>
#include <fixpp/meta.h>
#include <fixpp/dsl/details/unwrap.h>
#include <fixpp/dsl/details/flatten.h>


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

            template<typename T, typename = void> struct HasStrictMode : std::false_type { };
            template<typename T>
            struct HasStrictMode<
                    T,
                    void_t<decltype(&T::StrictMode)>
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
        void visitMessageType(const char* msgType, const char* version, size_t, Visitor visitor, Rules)
        {

            using Overrides = typename Rules::Overrides;

            using Version42 = Fix::v42::Version;
            using Version44 = Fix::v44::Version;

            if (Version42::equals(version))
            {
                using Header = Fix::v42::Header::Ref;

                switch (msgType[0])
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
                    case 'S':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v42::Message::Quote, Overrides>> {});
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
            else if (Version44::equals(version))
            {
                using Header = Fix::v44::Header::Ref;

                switch (msgType[0])
                {
                    case 'W':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v44::Message::MarketDataSnapshot, Overrides>> {});
                        break;
                }
            }
        }

        template<typename Field, typename Visitor>
        void doVisitSingleField(unsigned tag, Field& field, Visitor& visitor, bool* found)
        {
            if (tag == field.tag())
            {
                visitor(field);
                *found = true;
            }
        }

        template<typename Message, typename Visitor, size_t... Indexes>
        bool doVisitField(Message& message, unsigned tag, Visitor& visitor,
                          meta::index_sequence<Indexes...>)
        {
            bool found = false;
            int dummy[] = {0, (doVisitSingleField(tag, std::get<Indexes>(message.values), visitor, &found), 0)...};

            return found;
        }

        template<typename Message, typename Visitor>
        bool visitField(Message& message, unsigned tag, Visitor& visitor)
        {
            static constexpr size_t Size = Message::TotalTags;
            return doVisitField(message, tag, visitor, meta::make_index_sequence<Size>{});
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
               return std::make_pair(false, Ret {});

            TRY(match_literal('=', cursor));

            Ret ret;

            TRY(TagMatcher<typename Tag::Type>::matchValue(&ret, cursor));

            revert.ignore();

            return std::make_pair(true, ret);

            #undef TRY
        }

        namespace details
        {

             template<typename T, size_t Size>
             struct const_array
             {
                 T arr[Size];

                 constexpr const T& operator[](size_t index) const
                 {
                     return arr[index];
                 }

                 constexpr const T* begin() const { return arr; }
                 constexpr const T* end() const { return arr + Size; }
             };

             // return the index of the smallest element
             template<typename T, size_t Size>
             constexpr size_t const_min_index(const const_array<T, Size>& arr, size_t offset, size_t cur)
             {
                 return Size == offset ? cur :
                                        const_min_index(arr, offset + 1, arr[cur] < arr[offset] ? cur : offset);
             }

             // copy the array but with the elements at `index0` and `index1` swapped
             template<typename T, size_t Size, size_t... Is>
             constexpr const_array<T, Size> const_swap(const const_array<T, Size>& arr, size_t index0, size_t index1, meta::index_sequence<Is...>)
             {
                  return {{arr[Is == index0 ? index1 : Is == index1 ? index0 : Is]...}};
             }

            // the selection sort algorithm
            template<typename T, size_t Size>
            constexpr const_array<T, Size> const_selection_sort(const const_array<T, Size>& arr, size_t cur = 0)
            {
                return cur == Size ? arr :
                                     const_selection_sort(
                                           const_swap(arr, cur, const_min_index(arr, cur, cur), meta::make_index_sequence<Size>{}), cur+1 );
            }

            template<typename Tag>
            struct IndexOf
            {
                static constexpr int Value = Tag::Id;
            };

            template<typename Tag>
            struct IndexOf<Required<Tag>>
            {
                static constexpr int Value = Tag::Id;
            };

            template<typename GroupTag, typename... Tags>
            struct IndexOf<RepeatingGroup<GroupTag, Tags...>>
            {
                static constexpr int Value = GroupTag::Id;
            };

            template<typename GroupTag, typename... Tags>
            struct IndexOf<Required<RepeatingGroup<GroupTag, Tags...>>>
            {
                static constexpr int Value = GroupTag::Id;
            };

            template<typename Pack> struct IndexesImpl;

            template<typename... Tags>
            struct IndexesImpl<meta::pack::Pack<Tags...>>
            {

                static constexpr const_array<int, sizeof...(Tags)> Value = {
                    IndexOf<Tags>::Value...
                };

                static constexpr const_array<int, sizeof...(Tags)> Sorted = const_selection_sort(Value);

                static constexpr size_t Size = sizeof...(Tags);

                static int64_t of(int tag)
                {
                    return of_rec(tag, 0);
                }

            private:
                // TODO: binary search
                static constexpr int64_t of_rec(int tag, int64_t index)
                {
                    return (index == Size ? -1 :
                                (Sorted[index] == tag ? index : of_rec(tag, index + 1)));
                }
            };

            template<typename... Tags>
            constexpr const_array<int, sizeof...(Tags)> IndexesImpl<meta::pack::Pack<Tags...>>::Value;

            template<typename... Tags>
            constexpr const_array<int, sizeof...(Tags)> IndexesImpl<meta::pack::Pack<Tags...>>::Sorted;

            template<typename... Tags>
            struct MakeIndexes : public IndexesImpl<typename Fix::details::flatten::pack::Flatten<Tags...>::Result>
            {
            };
        };

        // ------------------------------------------------
        // TagSet
        // ------------------------------------------------

        // A bitset of valid tags inside a Message or RepeatingGroup

        template<typename... Tags>
        struct TagSet
        {
            // Indexes of tags in the Bitset
            using Indexes = details::MakeIndexes<Tags...>;

            void set(unsigned tag)
            {
                bits.set(Indexes::of(tag));
            }

            bool isset(unsigned tag)
            {
                return bits.test(Indexes::of(tag));
            }

            void reset()
            {
                bits.reset();
            }

            bool valid(unsigned tag)
            {
                return Indexes::of(tag) != -1;
            }

        private:
            std::bitset<Indexes::Size> bits;
        };

        template<typename VersionT, char MsgTypeChar, typename... Tags>
        struct TagSet<VersionnedMessage<VersionT, MsgTypeChar, Tags...>> : public TagSet<Tags...>
        {
        };

        template<typename GroupTag, typename... Tags>
        struct TagSet<RepeatingGroup<GroupTag, Tags...>> : public TagSet<Tags...>
        {
        };

        template<char MsgTypeChar, typename... Tags>
        struct TagSet<MessageRef<MsgTypeChar, Tags...>> : public TagSet<Tags...>
        {
        };

        template<template<typename> class FieldT, typename... Tags>
        struct TagSet<MessageBase<FieldT, Tags...>> : public TagSet<Tags...>
        {
        };


        // ------------------------------------------------
        // FieldParser
        // ------------------------------------------------

        template<typename Field> struct FieldParser;

        //
        // Specialization of our parser for a simple FieldRef
        // 

        template<typename TagT>
        struct FieldParser<FieldRef<TagT>>
        {
            using Field = FieldRef<TagT>;

            template<typename TagSet>
            void parse(Field& field, StreamCursor& cursor, TagSet& tagSet, bool /* strict */)
            {
                StreamCursor::Token valueToken(cursor);
                match_until('|', cursor);

                auto view = valueToken.view();
                cursor.advance(1);

                field.set(view);
                tagSet.set(field.tag());
            }
        };

        // ------------------------------------------------
        // FieldGroupVisitor
        // ------------------------------------------------

        //
        // Field parsing inside a RepeatingGroup
        //

        template<typename Field>
        struct FieldGroupVisitor
        {
            static constexpr bool Recursive = false;

            template<typename TagSet>
            void operator()(Field& field, const std::pair<const char*, size_t>& view,
                            StreamCursor&, TagSet&, bool)
            {
                field.set(view);
            }
        };

        //
        // Recursive RepeatingGroup parsing
        //

        template<typename GroupTag, typename... Tags>
        struct FieldGroupVisitor<FieldRef<RepeatingGroup<GroupTag, Tags...>>>
        {
            static constexpr bool Recursive = true;

            template<typename Field, typename TagSet>
            void operator()(Field& field, const std::pair<const char*, size_t>&,
                            StreamCursor& cursor, TagSet& outerSet, bool strict)
            {
                FieldParser<Field> parser;
                parser.parse(field, cursor, outerSet, strict);
            }
        };

        //
        // Specialization of our parser for a RepeatingGroup
        // 

        template<typename GroupTag, typename... Tags>
        struct FieldParser<FieldRef<RepeatingGroup<GroupTag, Tags...>>>
        {
            using Field = FieldRef<RepeatingGroup<GroupTag, Tags...>>;

            //
            // A GroupSet is a bitset of valid tags inside a RepeatingGroup
            // This allows us to know when:
            //   - We finished parsing an instance of a RepeatingGroup, as a
            //     bit for a given tag must already be set
            //   - We finished parsing the RepeatingGroup itself as a
            //     tag is not in the bitset
            //
            //
            using GroupSet = TagSet<Tags...>;

            struct Visitor
            {
                Visitor(const std::pair<const char*, size_t>& view, StreamCursor& cursor, GroupSet& groupSet, bool strict)
                    : view(view)
                    , cursor(cursor)
                    , groupSet(groupSet)
                    , strict(strict)
                    , recursive(0)
                { }

                template<typename Field>
                void operator()(Field& field)
                {
                    using GroupVisitor = FieldGroupVisitor<Field>;

                    GroupVisitor visitor;
                    visitor(field, view, cursor, groupSet, strict);

                    recursive = GroupVisitor::Recursive;
                }

                // Indicates whether we are in a recursive RepeatingGroup or not
                int recursive;

            private:
                std::pair<const char*, size_t> view;
                StreamCursor& cursor;
                GroupSet& groupSet;
                bool strict;
            };

            template<typename TagSet>
            void parse(Field& field, StreamCursor& cursor, TagSet& tagSet, bool strict)
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

                        // The tag we just encountered is invalid for the RepeatingGroup
                        if (!groupSet.valid(tag))
                        {
                            // If it's not valid for the Message either, we consider it to be
                            // a custom tag
                            if (!tagSet.valid(tag) && tag != 10)
                            {
                                // TODO: what should we do if we are in strict parsing ?
                                if (!strict)
                                {
                                    revertTag.ignore();
                                    cursor.advance(1);

                                    StreamCursor::Token valueToken(cursor);
                                    match_until('|', cursor);

                                    groupRef.unparsed.insert(std::make_pair(tag, valueToken.view()));

                                    cursor.advance(1);
                                    continue;
                                }
                            }

                            inGroup = false;
                            break;
                        }

                        // The tag is already set in our GroupSet, we finished parsing the current instance
                        if (groupSet.isset(tag))
                            break;

                        revertTag.ignore();
                        cursor.advance(1);

                        groupSet.set(tag);

                        StreamCursor::Token valueToken(cursor);
                        match_until('|', cursor);

                        auto view = valueToken.view();
                        Visitor visitor(view, cursor, groupSet, strict);

                        // Invariant: here visitField should ALWAYS return true are we are checking if the tag
                        // is valid prior to the call
                        //
                        // TODO: enfore the invariant ?
                        visitField(groupRef, tag, visitor);

                        // If we are in a recursive RepeatingGroup parsing, we must *NOT* advance the cursor
                        if (!visitor.recursive)
                            cursor.advance(1);
                    }

                    field.add(std::move(groupRef));
                    groupSet.reset();

                } while (inGroup);

            }
        };

        // ------------------------------------------------
        // FieldVisitor
        // ------------------------------------------------

        template<typename Message>
        struct FieldVisitor
        {
            FieldVisitor(StreamCursor& cursor, bool strict)
                : cursor(cursor)
                , strict(strict)
            { }

            template<typename Field>
            void operator()(Field& field)
            {
                TagSet<Message> tags;

                FieldParser<Field> parser;
                parser.parse(field, cursor, tags, strict);
            }

        private:
            StreamCursor& cursor;
            bool strict;
        };

        // ------------------------------------------------
        // MessageVisitor
        // ------------------------------------------------

        template<typename Visitor, typename Rules>
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

                enum class State {
                    InHeader,
                    InMessage
                };

                State state = State::InHeader;

                int checksum = 0;

                while (!cursor.eof())
                {
                    int tag;
                    match_int(&tag, cursor);
                    cursor.advance(1);

                    if (state == State::InHeader)
                    {
                        FieldVisitor<Header> headerVisitor(cursor, Rules::StrictMode);
                        if (visitField(header, tag, headerVisitor))
                            continue;

                        FieldVisitor<Message> messageVisitor(cursor, Rules::StrictMode);
                        if (visitField(message, tag, messageVisitor))
                        {
                            state = State::InMessage;
                            continue;
                        }
                    }
                    else if (state == State::InMessage)
                    {
                        FieldVisitor<Message> messageVisitor(cursor, Rules::StrictMode);
                        if (visitField(message, tag, messageVisitor))
                            continue;
                    }

                    if (tag == 10)
                    {
                        match_int(&checksum, cursor);
                    }
                    else
                    {
                        StreamCursor::Token valueToken(cursor);
                        match_until('|', cursor);

                        if (state == State::InHeader)
                            header.unparsed.insert(std::make_pair(tag, valueToken.view()));
                        else
                            message.unparsed.insert(std::make_pair(tag, valueToken.view()));
                    }

                    cursor.advance(1);

                }

                visitor(header, message);
            }

        private:
            Visitor visitor;
            StreamCursor& cursor;
        };

        template<typename Visitor, typename Rules>
        void visitMessage(const char* msgType, const char* version, size_t size, StreamCursor& cursor, Visitor, Rules rules)
        {
            MessageVisitor<Visitor, Rules> messageVisitor(cursor);
            visitMessageType(msgType, version, size, messageVisitor, rules);
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

        static constexpr bool StrictMode = false;
    };

    template<typename Visitor, typename Rules>
    void visit(const char* frame, size_t size, Visitor visitor, Rules rules)
    {
        static_assert(std::is_base_of<VisitRules, Rules>::value, "Visit rules must inherit from VisitRules");

        static_assert(impl::rules::HasOverrides<Rules>::value, "Visit rules must provide an Overrides typedef");
        static_assert(impl::rules::HasValidateChecksum<Rules>::value, "Visit rules must provide a static ValidateChecksum boolean");
        static_assert(impl::rules::HasValidateLength<Rules>::value, "Visit rules must provide a static ValidateLength boolean");
        static_assert(impl::rules::HasStrictMode<Rules>::value, "Visit rules must provide a static StrictMode boolean");

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

        impl::visitMessage(msgType.second.first, beginString.second.first, beginString.second.second, cursor, visitor, rules);
    }

    template<typename Visitor>
    void visit(const char* frame, size_t size, Visitor visitor)
    {
        visit(frame, size, visitor, DefaultRules {});
    }

} // namespace Fix
