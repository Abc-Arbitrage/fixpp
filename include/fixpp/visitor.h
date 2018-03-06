/* visitor.h
   Mathieu Stefani, 15 november 2016
   
  A typed visitor for a FIX frame
*/

#pragma once

#include <cstdio>
#include <cstdarg>
#include <type_traits>

#include <fixpp/tag.h>
#include <fixpp/utils/cursor.h>
#include <fixpp/utils/result.h>
#include <fixpp/utils/soh.h>
#include <fixpp/meta.h>
#include <fixpp/view.h>
#include <fixpp/dsl.h>
#include <fixpp/dsl/details/flatten.h>
#include <fixpp/dsl/details/lexical_cast.h>

namespace Fixpp
{


    // ------------------------------------------------
    // ErrorKind
    // ------------------------------------------------

    // Encapsulates types of errors that can occur during parsing.

    struct ErrorKind
    {
        enum Type { Incomplete, ParsingError, UnknownTag, UnknownMessage,
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

    template<typename T>
    using VisitError = result::Result<T, ErrorKind>;

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

    // ------------------------------------------------
    // ParsingContext
    // ------------------------------------------------

    // Keeps information related to the current parsing context like
    // MsgType or version and encapsulates the parsing error if any

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

        void setBodyLength(int length)
        {
            bodyLength = length;
        }

        void setMsgType(const View& view)
        {
            msgType = view;
        }

        void setVersion(const View& view)
        {
            version = view;
        }

        StreamCursor& cursor;
        Deferred<ErrorKind> error;

        View version;
        int bodyLength;
        View msgType;

        size_t sum;
    };

    // ------------------------------------------------
    // TypedParsingContext
    // ------------------------------------------------

    // Encapsulates the result of the parsing. Will encapsulate
    // the return type of the visitor when used through visit()
    // or the value of the tag when used through visitTag<T>()

    template<typename T>
    struct TypedParsingContext : public ParsingContext
    {
        using Type = T;

        TypedParsingContext(StreamCursor& cursor)
            : ParsingContext(cursor)
        { }

        void setValue(T value)
        {
            resultValue.construct(value);
        }

        VisitError<T> toVisitError() const
        {
            if (hasError())
                return result::Err(error.get());
            return result::Ok(resultValue.get());
        }

        Deferred<T> resultValue;
    };

    // Specialization for a void result

    template<>
    struct TypedParsingContext<void> : public ParsingContext
    {
        using Type = void;

        TypedParsingContext(StreamCursor& cursor)
            : ParsingContext(cursor)
        { }

        VisitError<void> toVisitError() const
        {
            if (hasError())
                return result::Err(error.get());
            return result::Ok();
        }
    };

    namespace impl
    {

        namespace rules
        {

            template<typename T> using HasOverrides = typename T::Overrides;
            template<typename T> using HasDictionary = typename T::Dictionary;
            template<typename T> using HasResultType = typename T::ResultType;
            template<typename T> using HasValidateChecksum = decltype(&T::ValidateChecksum);
            template<typename T> using HasValidateLength = decltype(&T::ValidateLength);
            template<typename T> using HasStrictMode = decltype(&T::StrictMode);
            template<typename T> using HasSkipUnknownTags = decltype(&T::SkipUnknownTags);

            template<typename Overrides>
            struct OverridesValidator;

            template<typename First, typename Second, typename... Rest>
            struct OverridesValidator<meta::map::Map<meta::map::Pair<First, Second>, Rest...>> : public OverridesValidator<meta::map::Map<Rest...>>
            {
                static_assert(std::is_same<typename First::MsgType, typename Second::MsgType>::value,
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

        // ------------------------------------------------
        // DictionaryVisitor
        // ------------------------------------------------

        // Visit a single message from the FIX Dictionary

        template<typename Header, typename Overrides>
        struct DictionaryVisitor
        {
            template<typename Message, typename Context, typename Visitor>
            static bool visit(const Context& context, Visitor& visitor)
            {
                auto msgType = context.msgType.first;
                auto msgTypeSize = context.msgType.second;
                if (Message::MsgType::equals(msgType, msgTypeSize))
                {
                    visitor(id<Header> { }, id<OverrideFor<Message, Overrides>> {});
                    return true;
                }

                return false;
            }
        };

        enum class VisitStatus
        {
            Ok,              // The version matched and the message has been found
            VersionMismatch, // The version did not match
            NotFound         // The version matched but the message could not be found in the typelist
        };

        // Every FIX version has a special Spec namespace that provides a list of structure
        // that represent a specific version of FIX. The Dictionary struct regroups the list
        // of all valid messages for a specific FIX version as well as the Version type itself.
        //
        // All valid messages for a version are encoded through a meta::typelist. Thus, at
        // run-time, after parsing the MsgType, we visit all the messages from the typelist
        // of the specific version until we find the corresponding one.
        //
        // That way, we only need to add new messages in this special typelist to make
        // them visible from the visitor
        
        template<typename Dictionary, typename Context, typename Visitor, typename Rules>
        VisitStatus visitDictionary(Context& context, Visitor& visitor, Rules)
        {
            using Version = typename Dictionary::Version;
            using Header = typename Dictionary::Header::Ref;
            using Messages = typename Dictionary::Messages;

            using Overrides = typename Rules::Overrides;

            auto version = context.version.first;
            auto versionSize = context.version.second;

            using MessageVisitor = DictionaryVisitor<Header, Overrides>;

            if (Version::equals(version, versionSize))
            {
                using AllMessagesVisitor = meta::typelist::ops::Visitor<Messages, MessageVisitor>;
                if (AllMessagesVisitor::visit(context, visitor))
                    return VisitStatus::Ok;

                return VisitStatus::NotFound;
            }

            return VisitStatus::VersionMismatch;
        }

        template<typename Visitor, typename Rules, typename Context>
        void visitMessage(Context& context, Visitor& visitor, Rules rules)
        {

            using Dictionary = typename Rules::Dictionary;
            using Version = typename Dictionary::Version;

            auto status = visitDictionary<Dictionary>(context, visitor, rules);

            if (status == VisitStatus::VersionMismatch)
            {
                std::string versionStr(context.version.first, context.version.second);

                context.setError(ErrorKind::InvalidVersion, "FIX version mismatched, expected '%s', got '%s'",
                        Version::Str, versionStr.c_str());
            }
            else if (status == VisitStatus::NotFound)
            {
                std::string versionStr(context.version.first, context.version.second);
                std::string msgTypeStr(context.msgType.first, context.msgType.second);

                context.setError(ErrorKind::UnknownMessage, "Unknown MsgType(%s) for FIX version %s", msgTypeStr.c_str(), versionStr.c_str());
            }
        }

        template<size_t Index, typename Field, typename Visitor>
        void doVisitSingleField(unsigned tag, Field& field, Visitor& visitor, int* index)
        {
            if (tag == field.tag())
            {
                visitor(field);
                *index = Index;
            }
        }

        template<typename Message, typename Visitor, size_t... Indexes>
        bool doVisitField(Message& message, unsigned tag, Visitor& visitor,
                          meta::seq::index_sequence<Indexes...>)
        {
            int index = -1;
            int dummy[] = {0, (doVisitSingleField<Indexes>(tag, meta::get<Indexes>(message.values), visitor, &index), 0)...};

            (void) dummy;
            (void) tag;
            (void) visitor;

            if (index != -1)
                message.allBits.set(static_cast<size_t>(index));

            return index != -1;
        }

        template<typename Message, typename Visitor>
        bool visitField(Message& message, unsigned tag, Visitor& visitor)
        {
            static constexpr size_t Size = Message::TotalTags;
            return doVisitField(message, tag, visitor, meta::seq::make_index_sequence<Size>{});
        }


#define TRY_ADVANCE(fmt, ...)                                                    \
    do {                                                                         \
        if (!cursor.advance(1))                                                  \
        {                                                                        \
            context.setError(ErrorKind::Incomplete, fmt, ## __VA_ARGS__);        \
            return;                                                              \
        }                                                                        \
    } while (0)

#define TRY_MATCH_INT(out, fmt, ...)                                              \
    do {                                                                          \
        if (!match_int_fast(&out, cursor))                                        \
        {                                                                         \
            context.setError(ErrorKind::ParsingError, fmt, ## __VA_ARGS__);       \
            return;                                                               \
        }                                                                         \
    } while (0)

#define TRY_MATCH_UNTIL(c, fmt, ...)                                              \
    do {                                                                          \
        if (!match_until_fast(c, cursor))                                         \
        {                                                                         \
            context.setError(ErrorKind::ParsingError, fmt, ## __VA_ARGS__);       \
            return;                                                               \
        }                                                                         \
    } while (0)

// @Improvement: Figure out how to properly print EOF
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
            using Return = View;

            static bool matchValue(Return* value, StreamCursor& cursor)
            {
                StreamCursor::Token valueToken(cursor);
                if (!match_until_fast(SOH, cursor)) return false;

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
                if (!match_int_fast(&val, cursor)) return false;

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
            TRY_MATCH(match_int_fast(&tag, cursor));

            if (tag != Tag::Id)
               return std::make_pair(false, Ret {});

            TRY_MATCH(match_literal_fast('=', cursor));

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
             constexpr const_array<T, Size> const_swap(const const_array<T, Size>& arr, size_t index0, size_t index1, meta::seq::index_sequence<Is...>)
             {
                  return {{arr[Is == index0 ? index1 : Is == index1 ? index0 : Is]...}};
             }

            // the selection sort algorithm
            template<typename T, size_t Size>
            constexpr const_array<T, Size> const_selection_sort(const const_array<T, Size>& arr, size_t cur = 0)
            {
                return cur == Size ? arr :
                                     const_selection_sort(
                                           const_swap(arr, cur, const_min_index(arr, cur, cur), meta::seq::make_index_sequence<Size>{}), cur+1 );
            }

            template<typename Tag>
            struct IndexOf
            {
                static constexpr int Value = Tag::Id;
            };

            template<typename Tag>
            struct IndexOf<FieldRef<Tag>>
            {
                static constexpr int Value = Tag::Id;
            };

            template<typename Tag>
            struct IndexOf<Required<Tag>>
            {
                static constexpr int Value = Tag::Id;
            };

            template<typename GroupTag, typename SizeHint, typename... Tags>
            struct IndexOf<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>
            {
                static constexpr int Value = GroupTag::Id;
            };

            template<typename GroupTag, typename SizeHint, typename... Tags>
            struct IndexOf<Required<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>>
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
                static constexpr int Max = Sorted[Size - 1];

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
            struct MakeIndexes : public IndexesImpl<typename Fixpp::details::flatten::pack::Flatten<Tags...>::Result>
            {
            };
        };

        // ------------------------------------------------
        // TagSet
        // ------------------------------------------------

        // A bitset of valid tags inside a Message or RepeatingGroup
        //
        // Note that to avoid index lookup when setting or testing
        // the value of a particular bit in the bitset, we "allocate"
        // a bitset large enough to hold the maximum tag value
        // (that is, if our TagSet has a tag numbered 10453, then
        //  our bitself will hold up to 10453 + 1 bits).
        //
        // While it should not be a problem in most cases, if tags
        // are large enough, we might cause a stack overflow as
        // std::bitset is using automatic (stack) storage.

        template<typename... Tags>
        struct TagSet
        {
            // Indexes of tags in the Bitset
            using Indexes = details::MakeIndexes<Tags...>;

            void set(unsigned tag)
            {
                bits.set(tag);
            }

            bool test(unsigned tag)
            {
                return bits.test(tag);
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
            std::bitset<Indexes::Max + 1> bits;
            // We can also use std::bitset<Indexes::Size> bits.
            // However, with that bitset, we need to lookup the bit
            // index for the tag any time we want to access a particular
            // bit in the bitset.
        };

        template<typename VersionT, typename Chars, typename... Tags>
        struct TagSet<VersionnedMessageRef<VersionT, Chars, Tags...>> : public TagSet<Tags...>
        {
        };

        template<typename GroupTag, typename SizeHint, typename... Tags>
        struct TagSet<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>> : public TagSet<Tags...>
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
            void parse(Field& field, ParsingContext& context, TagSet& tagSet, bool /* strict */, bool /* skipUnknown */)
            {
                auto& cursor = context.cursor;
                // @Todo: In Strict mode, validate the type of the Tag
                StreamCursor::Token valueToken(context.cursor);
                TRY_MATCH_UNTIL(SOH, "Expected value tag '%d', got EOF", TagT::Id);

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
            void operator()(Field& field,
                            ParsingContext& context, TagSet&, bool, bool)
            {
                static constexpr auto Tag = Field::Tag::Id;
                auto& cursor = context.cursor;

                StreamCursor::Token valueToken(cursor);
                TRY_MATCH_UNTIL(SOH, "Expected value after tag %d, got EOF", Tag);

                auto view = valueToken.view();

                field.set(view);
            }
        };

        //
        // Recursive RepeatingGroup parsing
        //

        template<typename GroupTag, typename SizeHint, typename... Tags>
        struct FieldGroupVisitor<FieldRef<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>>
        {
            static constexpr bool Recursive = true;

            template<typename Field, typename TagSet>
            void operator()(Field& field,
                            ParsingContext& context, TagSet& outerSet, bool strict, bool skipUnknown)
            {
                FieldParser<Field> parser;
                parser.parse(field, context, outerSet, strict, skipUnknown);
            }
        };

        //
        // Specialization of our parser for a RepeatingGroup
        // 

        template<typename GroupTag, typename SizeHint, typename... Tags>
        struct FieldParser<FieldRef<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>>
        {
            using Field = FieldRef<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>;

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
                Visitor(ParsingContext& context, GroupSet& groupSet, bool strict, bool skipUnknown)
                    : context(context)
                    , groupSet(groupSet)
                    , strict(strict)
                    , skipUnknown(skipUnknown)
                    , recursive(0)
                { }

                template<typename Field>
                void operator()(Field& field)
                {
                    using GroupVisitor = FieldGroupVisitor<Field>;

                    GroupVisitor visitor;
                    visitor(field, context, groupSet, strict, skipUnknown);

                    recursive = GroupVisitor::Recursive;
                }

                // Indicates whether we are in a recursive RepeatingGroup or not
                int recursive;

            private:
                ParsingContext& context;
                GroupSet& groupSet;
                bool strict;
                bool skipUnknown;
            };

            template<typename TagSet>
            void parse(Field& field, ParsingContext& context, TagSet& tagSet, bool strict, bool skipUnknown)
            {
                auto& cursor = context.cursor;

                int instances;
                TRY_MATCH_INT(
                    instances,
                    "Could not parse instances number in RepeatingGroup %d, expected int, got '%c'",
                     GroupTag::Id, CURSOR_CURRENT(cursor)
                );

                if (!instances)
                {
                    TRY_ADVANCE("Got early EOF after tag %d value", GroupTag::Id);
                    return;
                }

                // Literal('=')
                TRY_ADVANCE("Expected RepeatingGroup %d, got EOF", GroupTag::Id);

                field.reserve(instances);

                GroupSet groupSet;

                int tag;
                bool inGroup = true;

                do
                {
                    typename Field::GroupType groupRef;

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
                                TRY_MATCH_UNTIL(SOH, "Expected value after tag %d, got EOF", tag);

                                if (!strict)
                                {
                                    if (!skipUnknown)
                                        groupRef.unparsed.emplace_back(tag, valueToken.view());
                                    TRY_ADVANCE("Got early EOF after tag %d value", tag);
                                    continue;
                                }
                                else if (strict)
                                {
                                    context.setError(ErrorKind::UnknownTag, "Encountered unknown tag %d in RepeatingGroup %d", tag, GroupTag::Id);
                                    return;
                                }
                            }

                            inGroup = false;
                            break;
                        }

                        // The tag is already set in our GroupSet, we finished parsing the current instance
                        if (groupSet.test(tag))
                            break;

                        revertTag.ignore();

                        // Literal('=')
                        TRY_ADVANCE("Expected value after Tag %d, got EOF", tag);

                        groupSet.set(tag);

                        Visitor visitor(context, groupSet, strict, skipUnknown);

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
            FieldVisitor(ParsingContext& context, bool strict, bool skipUnknown)
                : context(context)
                , strict(strict)
                , skipUnknown(skipUnknown)
            { }

            template<typename Field>
            void operator()(Field& field)
            {
                TagSet<Message> tags;

                FieldParser<Field> parser;
                parser.parse(field, context, tags, strict, skipUnknown);
            }

        private:
            ParsingContext& context;
            bool strict;
            bool skipUnknown;
        };

        // ------------------------------------------------
        // MessageVisitor
        // ------------------------------------------------

        template<typename Visitor, typename Rules>
        struct MessageVisitor
        {

            using ResultType = typename Visitor::ResultType;
            using Context = TypedParsingContext<ResultType>;

            template<bool ...Bools> using BoolPack = meta::pack::ValuePack<bool, Bools...>;

            MessageVisitor(Context& context, Visitor& visitor)
                : context(context)
                , visitor(visitor)
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
                        FieldVisitor<Header> headerVisitor(context, Rules::StrictMode, Rules::SkipUnknownTags);
                        if (visitField(header, tag, headerVisitor))
                            continue;

                        FieldVisitor<Message> messageVisitor(context, Rules::StrictMode, Rules::SkipUnknownTags);
                        if (visitField(message, tag, messageVisitor))
                        {
                            state = State::InMessage;
                            continue;
                        }
                    }
                    else if (state == State::InMessage)
                    {
                        FieldVisitor<Message> messageVisitor(context, Rules::StrictMode, Rules::SkipUnknownTags);
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
                        break;
                    }
                    else
                    {
                        StreamCursor::Token valueToken(cursor);
                        TRY_MATCH_UNTIL(SOH, "Expected value after tag %d, got EOF", tag);

                        if (state == State::InHeader)
                            handleUnknownTag(valueToken, header, context, tag,
                                    BoolPack<Rules::SkipUnknownTags, Rules::StrictMode>{});
                        else
                            handleUnknownTag(valueToken, message, context, tag,
                                    BoolPack<Rules::SkipUnknownTags, Rules::StrictMode>{});

                    }

                    TRY_ADVANCE("Got early EOF");

                }

                if (!hasError())
                    callVisitor(header, message, std::is_void<typename Context::Type>{});
            }

            bool hasError() const
            {
                return context.hasError();
            }

        private:

            // not(SkipUnknownTags) and not(StrictMode)
            template<typename Message, typename Context>
            void handleUnknownTag(const StreamCursor::Token& valueToken, Message& message, Context& /*context*/, int tag,
                                  BoolPack<false, false>)
            {
                message.unparsed.emplace_back(tag, valueToken.view());
            }

            // SkipUnknownTags and StrictMode, SkipUnknownTags wins ?
            template<typename Message, typename Context>
            void handleUnknownTag(const StreamCursor::Token& /*valueToken*/, Message& /*message*/, Context& /*context*/, int /*tag*/,
                                  BoolPack<true, true>)
            {
            }

            // SkipUnknownTags and not(StrictMode)
            template<typename Message, typename Context>
            void handleUnknownTag(const StreamCursor::Token& /*valueToken*/, Message& /*message*/, Context& /*context*/, int /*tag*/,
                                  BoolPack<true, false>)
            {
            }

            // not(SkipUnknownTags) and StrictMode
            template<typename Message, typename Context>
            void handleUnknownTag(const StreamCursor::Token& /*valueToken*/, Message& /*message*/, Context& context, int tag,
                                  BoolPack<false, true>)
            {
                context.setError(ErrorKind::UnknownTag, "Encountered unknown tag %d", tag);
            }

            template<typename Header, typename Message>
            void callVisitor(const Header& header, const Message& message, std::true_type /* is_void */)
            {
                visitor(header, message);
            }

            template<typename Header, typename Message>
            void callVisitor(const Header& header, const Message& message, std::false_type /* is_void */)
            {
                auto res = visitor(header, message);
                context.setValue(res);
            }

            Visitor& visitor;
            Context& context;
        };

    } // namespace impl

    struct VisitRules
    {
        template<typename... Args>
        using OverrideSet = meta::map::Map<Args...>;

        template<typename First, typename Second>
        using Override = meta::map::Pair<First, Second>;

        template<typename T> using As = T;
    };


    template<typename T>
    struct StaticVisitor
    {
        using ResultType = T;
    };

    template<typename Visitor, typename Rules, typename Context>
    void visitMessage(Context& context, Visitor& visitor, Rules rules)
    {
        auto& cursor = context.cursor;
        auto beginString = impl::matchTag<Tag::BeginString>(cursor);

        TRY_ADVANCE("Expected BodyLength after BeginString, got EOFs");

        auto bodyLength = impl::matchTag<Tag::BodyLength>(cursor);
        if (!bodyLength.first)
        {
            context.setError(ErrorKind::ParsingError,
                             "Could not parse BodyLength, expected int, got '%c'",
                              CURSOR_CURRENT(cursor));
            return;
        }

        TRY_ADVANCE("Expected MsgType after BodyLength, got EOF");
        auto msgType = impl::matchTag<Tag::MsgType>(cursor);
        TRY_ADVANCE("Expected Header after MsgType, got EOF");

        context.setVersion(beginString.second);
        context.setBodyLength(bodyLength.second);
        context.setMsgType(msgType.second);

        impl::MessageVisitor<Visitor, Rules> messageVisitor(context, visitor);
        impl::visitMessage(context, messageVisitor, rules);
    }

    template<typename Rules>
    static void checkRules()
    {
        static_assert(std::is_base_of<VisitRules, Rules>::value, "Visit rules must inherit from VisitRules");

        static_assert(
            meta::is_detected<impl::rules::HasOverrides, Rules>::value,
            "Visit rules must provide an Overrides typedef"
        );
        static_assert(
            meta::is_detected<impl::rules::HasDictionary, Rules>::value,
            "Visit rules must provide a Dictionary typedef"
        );
        static_assert(
            meta::is_detected<impl::rules::HasValidateChecksum, Rules>::value,
            "Visit rules must provide a static ValidateChecksum boolean"
        );
        static_assert(
            meta::is_detected<impl::rules::HasValidateLength, Rules>::value,
            "Visit rules must provide a static ValidateLength boolean"
        );
        static_assert(
            meta::is_detected<impl::rules::HasStrictMode, Rules>::value,
            "Visit rules must provide a static StrictMode boolean"
        );
        static_assert(
            meta::is_detected<impl::rules::HasSkipUnknownTags, Rules>::value,
            "Visit rules must provide a static SkipUnknownTags boolean"
        );

        impl::rules::OverridesValidator<typename Rules::Overrides> validator {};
    }


    // Note that the Visitor is passed by lvalue-reference, which means that is currently
    // not possible to pass an rvalue like a lambda or a bind-expression as a Visitor.
    // @Investigate a way to make it possible as it will be useful with polymorphic lambdas

    template<typename Visitor, typename Rules>
    auto visit(const char* frame, size_t size, Visitor& visitor, Rules rules) -> VisitError<typename Visitor::ResultType>
    {
        static_assert(
            meta::is_detected<impl::rules::HasResultType, Visitor>::value,
            "Visitor must fulfill StaticVisitor requirement and must expose an inner ResultType type"
        );
        checkRules<Rules>();

        RawStreamBuf<> streambuf(const_cast<char *>(frame), size);
        StreamCursor cursor(&streambuf);

        using ResultType = typename Visitor::ResultType;

        TypedParsingContext<ResultType> context(cursor);
        visitMessage(context, visitor, rules);

        return context.toVisitError();
    }

    template<typename Tag>
    VisitError<View> visitTagView(const char* frame, size_t size)
    {
        RawStreamBuf<> streambuf(const_cast<char *>(frame), size);
        StreamCursor cursor(&streambuf);

        TypedParsingContext<View> context(cursor);

        auto doVisitTag = [&]() {
            do
            {
                int tag;
                TRY_MATCH_INT(
                    tag,
                    "Encountered invalid tag, expected int, got '%c'",
                    CURSOR_CURRENT(cursor)
                );
                TRY_ADVANCE("Expected value after tag %d, got EOF", tag);

                StreamCursor::Token valueToken(cursor);
                TRY_MATCH_UNTIL(SOH, "Expected value after tag %d, got EOF", tag);

                if (tag == Tag::Id)
                {
                    context.setValue(valueToken.view());
                    return;
                }

                cursor.advance(1);

            } while (!cursor.eof() && !context.hasError());

            context.setError(ErrorKind::UnknownTag, "Could not find tag %d in given frame", Tag::Id);
        };

        doVisitTag();

        return context.toVisitError();
    }

    template<typename Tag>
    auto visitTag(const char* frame, size_t size) -> VisitError<typename Tag::Type::UnderlyingType>
    {
        return visitTagView<Tag>(frame, size).map([](const View& view)
        {
            return details::LexicalCast<typename Tag::Type>::cast(view.first, view.second);
        });
    }

#undef TRY_ADVANCE
#undef TRY_MATCH_INT
#undef TRY_MATCH_UNTIL

} // namespace Fixpp
