/* visitor.h
   Mathieu Stefani, 15 november 2016
   
  A typed visitor for a FIX frame
*/

#pragma once

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <type_traits>
#include <algorithm>

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>

#include <fixpp/tag.h>
#include <fixpp/utils/cursor.h>
#include <fixpp/utils/result.h>
#include <fixpp/meta.h>
#include <fixpp/dsl/details/unwrap.h>
#include <fixpp/dsl/details/flatten.h>

namespace Fix
{

    namespace details
    {

        template<int T>
        struct TaggedEnum { };
    };

    // ------------------------------------------------
    // ErrorKind
    // ------------------------------------------------

    // Encapsulates types of errors that can occur during parsing.

    struct ErrorKind
    {
        enum Type { Incomplete, ParsingError, UnknownTag,
                    InvalidVersion, InvalidTag, InvalidChecksum, InvalidLength };

        ErrorKind(Type type, size_t column, std::string str)
            : type_(type)
            , column_(column)
            , str_(std::move(str))
        { }

        Type type() const
        {
            return type_;
        }

        std::string asString() const
        {
            return str_;
        }

        size_t column() const
        {
            return column_;
        }

    private:
        Type type_;
        size_t column_;
        std::string str_;
    };

    using VisitError = Result<void, ErrorKind>;

    VisitError makeError(ErrorKind::Type type, const StreamCursor& cursor, const char* errFmt, ...)
    {
        const size_t column = static_cast<size_t>(cursor);

        char error[255];
        std::memset(error, 0, sizeof error);
        va_list args;
        va_start(args, errFmt);
        auto count = std::vsnprintf(error, sizeof error, errFmt, args);
        va_end(args);

        return Err(ErrorKind(type, column, std::string(error, count)));
    }

    namespace impl
    {

        // ------------------------------------------------
        // Deferred
        // ------------------------------------------------

        // A sample helper template to defer construction
        // of a certain type (lightweight optional)

        template<typename T>
        struct Deferred
        {
            Deferred()
                : empty_(true)
            { }

            typedef typename std::aligned_storage<sizeof (T), alignof (T)>::type Storage;

            template<typename... Args>
            void construct(Args&& ...args)
            {
                new (&storage_) T(std::forward<Args>(args)...);
                empty_ = false;
            }

            bool isEmpty() const
            {
                return empty_;
            }

            const T& get() const
            {
                // @Safety Assert that empty_ is false
                return *reinterpret_cast<const T *>(&storage_);
            }

            T* operator->()
            {
                // @Safety Assert that empty_ is false
                return reinterpret_cast<T *>(&storage_);
            }

            const T* operator->() const
            {
                // @Safety Assert that empty_ is false
                return reinterpret_cast<const T*>(&storage_);
            }

        private:
            Storage storage_;
            bool empty_;
        };

        struct ParsingContext
        {
            ParsingContext(StreamCursor& cursor)
                : cursor(cursor)
                , sum(0)
            { }

            void setError(ErrorKind::Type type, const char* errFmt, ...)
            {
                auto column = static_cast<size_t>(cursor);

                char errStr[255];
                std::memset(errStr, 0, sizeof errStr);
                va_list args;
                va_start(args, errFmt);
                auto count = std::vsnprintf(errStr, sizeof errStr, errFmt, args);
                va_end(args);

                error.construct(type, column, std::string(errStr, count));
            }

            bool hasError() const
            {
                return !error.isEmpty();
            }

            StreamCursor& cursor;
            Deferred<ErrorKind> error;
            size_t sum;
        };

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
                static_assert(std::is_same<typename First::MsgTypeChars, typename Second::MsgTypeChars>::value,
                              "Invalid Override: MsgType must be the same");
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
        VisitError visitMessageType(StreamCursor& cursor, const char* msgType, const char* version, size_t size, Visitor& visitor, Rules)
        {

            using Overrides = typename Rules::Overrides;

            using Version42 = Fix::v42::Version;
            using Version44 = Fix::v44::Version;

            if (Version42::equals(version, size))
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
            else if (Version44::equals(version, size))
            {
                using Header = Fix::v44::Header::Ref;

                switch (msgType[0])
                {
                    case 'W':
                        visitor(id<Header> {}, id<OverrideFor<Fix::v44::Message::MarketDataSnapshot, Overrides>> {});
                        break;
                }
            }
            else
            {
                return makeError(ErrorKind::InvalidVersion, cursor, "Got invalid FIX version '%s'", version);
            }

            if (visitor.hasError())
                return Err(visitor.error());

            return Ok();
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

#define TRY_ADVANCE(fmt, ...)                                           \
    do {                                                                \
        if (!cursor.advance(1))                                         \
        {                                                               \
            context.setError(ErrorKind::Incomplete, fmt, __VA_ARGS__);  \
            return;                                                     \
        }                                                               \
    } while (0)

#define TRY_MATCH_INT(out, fmt, ...)                             \
    do {                                                      \
        if (!match_int(&out, cursor))                         \
        {                                                     \
            context.setError(ErrorKind::ParsingError, fmt, __VA_ARGS__); \
            return;                                           \
        }                                                     \
    } while (0)

#define TRY_MATCH_UNTIL(c, fmt, ...)                             \
    do {                                                      \
        if (!match_until(c, cursor))                          \
        {                                                     \
            context.setError(ErrorKind::ParsingError, fmt, __VA_ARGS__); \
            return;                                           \
        }                                                     \
    } while (0)

#define CURSOR_CURRENT(c) (c.eof() ? '?' : c.current())

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
            #define TRY_MATCH(...) \
            if (!__VA_ARGS__) \
               return std::make_pair(false, Ret {})

            StreamCursor::Revert revert(cursor);
            
            int tag;
            TRY_MATCH(match_int(&tag, cursor));
            
            if (tag != Tag::Id)
               return std::make_pair(false, Ret {});

            TRY_MATCH(match_literal('=', cursor));

            Ret ret;

            TRY_MATCH(TagMatcher<typename Tag::Type>::matchValue(&ret, cursor));

            revert.ignore();

            return std::make_pair(true, ret);

            #undef TRY_MATCH
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
                    //return of_rec(tag, 0);
                    return of_binary_search(tag);
                }


            private:
                static int64_t of_binary_search(int tag)
                {
                    int64_t low = 0;
                    int64_t high = Size - 1;
                    while (low <= high) {
                       auto mid = (low + high) / 2;
                       if (Sorted[mid] == tag) return mid;
                       else if (Sorted[mid] < tag) low = mid + 1;
                       else high = mid - 1;
                    }
                    return -1;
                }

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

        template<typename VersionT, typename Chars, typename... Tags>
        struct TagSet<VersionnedMessage<VersionT, Chars, Tags...>> : public TagSet<Tags...>
        {
        };

        template<typename GroupTag, typename... Tags>
        struct TagSet<RepeatingGroup<GroupTag, Tags...>> : public TagSet<Tags...>
        {
        };

        template<typename Chars, typename... Tags>
        struct TagSet<MessageRef<Chars, Tags...>> : public TagSet<Tags...>
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
            void parse(Field& field, ParsingContext& context, TagSet& tagSet, bool /* strict */)
            {
                auto& cursor = context.cursor;
                // @Todo: In Strict mode, validate the type of the Tag
                StreamCursor::Token valueToken(context.cursor);
                TRY_MATCH_UNTIL('|', "Expected value tag '%d', got EOF", TagT::Id);

                auto view = valueToken.view();
                TRY_ADVANCE("Expected value after tag '%d', got EOF", TagT::Id);

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
                            ParsingContext&, TagSet&, bool)
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
                            ParsingContext& context, TagSet& outerSet, bool strict)
            {
                FieldParser<Field> parser;
                parser.parse(field, context, outerSet, strict);
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
                Visitor(const std::pair<const char*, size_t>& view, ParsingContext& context, GroupSet& groupSet, bool strict)
                    : view(view)
                    , context(context)
                    , groupSet(groupSet)
                    , strict(strict)
                    , recursive(0)
                { }

                template<typename Field>
                void operator()(Field& field)
                {
                    using GroupVisitor = FieldGroupVisitor<Field>;

                    GroupVisitor visitor;
                    visitor(field, view, context, groupSet, strict);

                    recursive = GroupVisitor::Recursive;
                }

                // Indicates whether we are in a recursive RepeatingGroup or not
                int recursive;

            private:
                std::pair<const char*, size_t> view;
                ParsingContext& context;
                GroupSet& groupSet;
                bool strict;
            };

            template<typename TagSet>
            void parse(Field& field, ParsingContext& context, TagSet& tagSet, bool strict)
            {
                auto& cursor = context.cursor;

                int instances;
                TRY_MATCH_INT(
                    instances,
                    "Could not parse instances number in RepeatingGroup %d, expected int, got '%c'",
                     GroupTag::Id, CURSOR_CURRENT(cursor)
                );
                TRY_ADVANCE("Expected RepeatingGroup %d, got EOF", GroupTag::Id);

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
                        TRY_MATCH_INT(
                            tag,
                            "Could not parse tag in RepeatingGroup %d, expected int, got '%c'",
                            GroupTag::Id, CURSOR_CURRENT(cursor)
                        );

                        // The tag we just encountered is invalid for the RepeatingGroup
                        if (!groupSet.valid(tag))
                        {
                            // If it's not valid for the Message either, we consider it to be
                            // a custom tag
                            if (!tagSet.valid(tag) && tag != 10)
                            {
                                revertTag.ignore();
                                TRY_ADVANCE("Expected value after tag %d, got EOF", tag);

                                StreamCursor::Token valueToken(cursor);
                                TRY_MATCH_UNTIL('|', "Expected value after tag %d, got EOF", tag);

                                if (!strict)
                                {
                                    groupRef.unparsed.insert(std::make_pair(tag, valueToken.view()));
                                    TRY_ADVANCE("Got early EOF after tag %d value", tag);
                                    continue;
                                }
                                else
                                {
                                    context.setError(ErrorKind::UnknownTag, "Encountered unknown tag %d in RepeatingGroup %d", tag, GroupTag::Id);
                                    return;
                                }
                            }

                            inGroup = false;
                            break;
                        }

                        // The tag is already set in our GroupSet, we finished parsing the current instance
                        if (groupSet.isset(tag))
                            break;

                        revertTag.ignore();
                        TRY_ADVANCE("Expected value after Tag %d, got EOF", tag);

                        groupSet.set(tag);

                        StreamCursor::Token valueToken(cursor);
                        TRY_MATCH_UNTIL('|', "Expected value after tag %d, got EOF", tag);

                        auto view = valueToken.view();
                        Visitor visitor(view, context, groupSet, strict);

                        // Invariant: here visitField should ALWAYS return true as we are checking if the tag
                        // is valid prior to the call
                        //
                        // TODO: enfore the invariant ?
                        visitField(groupRef, tag, visitor);

                        // If we are in a recursive RepeatingGroup parsing, we must *NOT* advance the cursor
                        if (!visitor.recursive)
                            TRY_ADVANCE("Got early EOF in RepeatingGroup %d", GroupTag::Id);
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
            FieldVisitor(ParsingContext& context, bool strict)
                : context(context)
                , strict(strict)
            { }

            template<typename Field>
            void operator()(Field& field)
            {
                TagSet<Message> tags;

                FieldParser<Field> parser;
                parser.parse(field, context, tags, strict);
            }

        private:
            ParsingContext& context;
            bool strict;
        };

        // ------------------------------------------------
        // MessageVisitor
        // ------------------------------------------------

        template<typename Visitor, typename Rules>
        struct MessageVisitor
        {
            MessageVisitor(StreamCursor& cursor)
                : context(cursor)
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

                auto& cursor = context.cursor;

                while (!cursor.eof() && !hasError())
                {
                    int tag;

                    TRY_MATCH_INT(
                        tag,
                        "Encountered invalid tag, expected int, got '%c'",
                        CURSOR_CURRENT(cursor)
                    );
                    TRY_ADVANCE("Expected value after Tag %d, got EOF", tag);

                    if (state == State::InHeader)
                    {
                        FieldVisitor<Header> headerVisitor(context, Rules::StrictMode);
                        if (visitField(header, tag, headerVisitor))
                            continue;

                        FieldVisitor<Message> messageVisitor(context, Rules::StrictMode);
                        if (visitField(message, tag, messageVisitor))
                        {
                            state = State::InMessage;
                            continue;
                        }
                    }
                    else if (state == State::InMessage)
                    {
                        FieldVisitor<Message> messageVisitor(context, Rules::StrictMode);
                        if (visitField(message, tag, messageVisitor))
                            continue;
                    }

                    if (tag == 10)
                    {
                        TRY_MATCH_INT(
                            checksum,
                            "Invalid checksum, expected int, got '%c'",
                             CURSOR_CURRENT(cursor)
                        );
                    }
                    else
                    {
                        StreamCursor::Token valueToken(cursor);
                        TRY_MATCH_UNTIL('|', "Expected value after tag %d, got EOF", tag);

                        if (!Rules::StrictMode)
                        {
                            if (state == State::InHeader)
                                header.unparsed.insert(std::make_pair(tag, valueToken.view()));
                            else
                                message.unparsed.insert(std::make_pair(tag, valueToken.view()));
                        }
                        else
                        {
                            context.setError(ErrorKind::UnknownTag, "Encountered unknown tag %d", tag);
                            break;
                        }
                    }

                    // Dirty hack to force the expansion of ellipsis in the macro
                    TRY_ADVANCE("Got early %s", "EOF");

                }

                if (!hasError())
                    visitor(header, message);
            }

            bool hasError() const
            {
                return context.hasError();
            }

            ErrorKind error() const
            {
                return context.error.get();
            }

        private:
            Visitor visitor;
            ParsingContext context;
        };

        template<typename Visitor, typename Rules>
        VisitError visitMessage(const char* msgType, const char* version, size_t size, StreamCursor& cursor, Visitor, Rules rules)
        {
            MessageVisitor<Visitor, Rules> messageVisitor(cursor);
            return visitMessageType(cursor, msgType, version, size, messageVisitor, rules);
        }

#undef TRY_ADVANCE
#undef TRY_MATCH_INT
#undef TRY_MATCH_UNTIL

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
    VisitError visit(const char* frame, size_t size, Visitor visitor, Rules rules)
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
            return makeError(ErrorKind::Incomplete, cursor, "Expected BodyLength after BeginString, got EOF");

        if (!impl::matchTag<Tag::BodyLength>(cursor).first)
            return makeError(ErrorKind::InvalidTag, cursor, "Could not parse BodyLength, expected int, got '%c'", CURSOR_CURRENT(cursor));

        if (!cursor.advance(1))
            return makeError(ErrorKind::ParsingError, cursor, "Expected MsgType after BodyLength, got EOF");

        auto msgType = impl::matchTag<Tag::MsgType>(cursor);
        
        if (!cursor.advance(1))
            return makeError(ErrorKind::ParsingError, cursor, "Expected Header after MsgType, got EOF");

        return impl::visitMessage(msgType.second.first, beginString.second.first, beginString.second.second, cursor, visitor, rules);
    }

    template<typename Visitor>
    VisitError visit(const char* frame, size_t size, Visitor visitor)
    {
        return visit(frame, size, visitor, DefaultRules {});
    }

} // namespace Fix
