/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX message as a collection of tags
*/

#pragma once

#include <tuple>
#include <unordered_map>
#include <bitset>

#include <fixpp/view.h>
#include <fixpp/dsl/details/chars.h>
#include <fixpp/meta/list.h>
#include <fixpp/meta/tuple.h>

#include <fixpp/dsl/details/unwrap.h>
#include <fixpp/dsl/details/traits.h>
#include <fixpp/dsl/details/flatten.h>
#include <fixpp/utils/SmallVector.h>

namespace Fixpp
{

    // ------------------------------------------------
    // MessageBase
    // ------------------------------------------------

    // Stores the list of all fields inside a std::tuple
    //
    namespace details
    {
        template<typename Tag, int CurrentMax>
        struct Max
        {
            static constexpr int Result = Tag::Id > CurrentMax ? Tag::Id : CurrentMax;
        };

        template<typename Tag, int CurrentMax>
        struct Max<Required<Tag>, CurrentMax>
        {
            static constexpr int Result = Tag::Id > CurrentMax ? Tag::Id : CurrentMax;
        };

        template<typename GroupTag, typename SizeHint, typename... Tags, int CurrentMax>
        struct Max<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>, CurrentMax>
        {
            static constexpr int Result = GroupTag::Id > CurrentMax ? GroupTag::Id : CurrentMax;
        };

        template<typename GroupTag, typename SizeHint, typename... Tags, int CurrentMax>
        struct Max<Required<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>, CurrentMax>
        {
            static constexpr int Result = GroupTag::Id > CurrentMax ? GroupTag::Id : CurrentMax;
        };
    };

    template<template<typename> class FieldT, typename... Tags> struct MessageBase
    {
        using Fields = typename details::Flattened<FieldT, Tags...>::Fields;
        using List = typename details::Flattened<FieldT, Tags...>::List;

        using TagsList = typename meta::typelist::ops::Map<List, details::Unwrap>::Result;

        //
        // We first need to filter-out all the Required tags. However, the result of the
        // filter operation will give us a typelist of Required<Tag> tags.
        // We then need to 'unwrap' the Tag to get a final typelist of Tag. Thus, we
        // call Map and unwrap the tag.
        //
        // Summary: RequiredList will be a TypeList<Tag1, Tag2, Tag3>, not TypeList<Required<Tag1>, ...>
        //
        using RequiredList =
            typename meta::typelist::ops::Map<
            typename meta::typelist::ops::Filter<List, details::IsRequired>::Result,
            details::Unwrap
            >::Result;

        using Ref = MessageBase<FieldRef, Tags...>;

        static constexpr size_t RequiredTags = meta::typelist::ops::Length<RequiredList>::value;
        // Note that TotalTags is *NOT* sizeof...(Tags) as we might have ComponentBlocks that
        // we flattened here. See flatten.h for more details
        static constexpr size_t TotalTags = meta::typelist::ops::Length<TagsList>::value;

        static constexpr int MaxTag = meta::typelist::ops::Fold<
            List, int, 0, details::Max
        >::Value;

        Fields values;

        std::bitset<RequiredTags> requiredBits;
        std::bitset<TotalTags> allBits;

        using View = std::pair<const char*, size_t>;
        struct Unparsed
        {
            Unparsed(int tag, const View& view)
                : tag(tag)
                , view(view)
            { }

            int tag;
            View view;
        };

        llvm::SmallVector<Unparsed, 10> unparsed;
    };

    // ------------------------------------------------
    // MessageRef
    // ------------------------------------------------

    // A "view" on a Message
    //

    template<typename MsgType, typename... Tags> struct MessageRef;

    template<char... MsgTypeChar, typename... Tags>
    struct MessageRef<Chars<MsgTypeChar...>, Tags...> : public MessageBase<FieldRef, Tags...>
    {
        using MsgType = Chars<MsgTypeChar...>;
    };

    template<typename... Tags>
    struct MessageRef<Empty, Tags...> : public MessageBase<FieldRef, Tags...>
    {
    };

    // ------------------------------------------------
    // MessageT
    // ------------------------------------------------

    // A real Message with its MsgType
    //
    template<typename MsgType, typename... Tags> struct MessageT;

    template<char... MsgTypeChar, typename... Tags>
    struct MessageT<Chars<MsgTypeChar...>, Tags...> : public MessageBase<Field, Tags...>
    {
        using MsgType = Chars<MsgTypeChar...>;
    };

    template<typename... Tags>
    struct MessageT<Empty, Tags...> : public MessageBase<Field, Tags...>
    {
    };

    // ------------------------------------------------
    // VersionnedMessage
    // ------------------------------------------------

    // A Message that knows its FIX version

    template<typename VersionT, typename Chars, typename... Tags>
    struct VersionnedMessageRef : public MessageRef<Chars, Tags...>
    {
        using Version = VersionT;
    };

    template<typename VersionT, typename Chars, typename... Tags>
    struct VersionnedMessage : public MessageT<Chars, Tags...>
    {
        using Ref = VersionnedMessageRef<VersionT, Chars, Tags...>;
        using Version = VersionT;
    };

    namespace details
    {
        template<typename Message>
        struct FromRef;

        template<typename VersionT, typename Chars, typename... Tags>
        struct FromRef<VersionnedMessageRef<VersionT, Chars, Tags...>>
        {
            using Message = VersionnedMessage<VersionT, Chars, Tags...>;
            using Ref = VersionnedMessageRef<VersionT, Chars, Tags...>;

            template<typename FieldRef, typename = void>
            struct FieldRefCast
            {
                template<typename Field>
                static void cast(const FieldRef& src, Field& dst)
                {
                    dst.set(src.get());
                }
            };

            template<typename GroupTag, typename SizeHint, typename... GroupTags>
            struct FieldRefCast<FieldRef<SmallRepeatingGroup<GroupTag, SizeHint, GroupTags...>>, void>
            {
                template<typename FieldRef, typename Field>
                static void cast(const FieldRef& src, Field& dst)
                {
                    const auto& srcInstances = src.get();
                    auto& dstInstances = dst.get();
                    dstInstances.resize(src.size());

                    for (size_t i = 0; i < srcInstances.size(); ++i)
                    {
                        copyFields(srcInstances[i], dstInstances[i]);
                    }
                }

                template<typename FieldRef, typename Field>
                static void copyFields(const FieldRef& src, Field& dst)
                {
                    copyInstanceFields(src, dst, meta::seq::make_index_sequence<FieldRef::TotalTags>());
                }

                template<typename FieldRef, typename Field, size_t ... Indexes>
                static void copyInstanceFields(const FieldRef& src, Field& dst, meta::seq::index_sequence<Indexes...>)
                {
                    int dummy[] = {0, ((void) copyInstanceFieldAt<Indexes>(src, dst), 0)...};
                    (void) dummy;
                }

                template<size_t Index, typename FieldRef, typename Field>
                static void copyInstanceFieldAt(const FieldRef& src, Field& dst)
                {
                    if (src.allBits.test(Index))
                    {
                        const auto& srcField = meta::get<Index>(src.values);
                        auto& dstField = meta::get<Index>(dst.values);
                        copyInstanceField(srcField, dstField);
                        dst.allBits.set(Index);
                    }
                }

                template<typename FieldRef, typename Field>
                static void copyInstanceField(const FieldRef& src, Field& dst)
                {
                    FieldRefCast<FieldRef>::cast(src, dst);
                }
            };

            static Message fromRef(const Ref& ref)
            {
                Message message;
                copyFields(ref, message, meta::seq::make_index_sequence<Ref::TotalTags>());
                return message;
            }

            template<size_t... Indexes>
            static void copyFields(const Ref& ref, Message& message, meta::seq::index_sequence<Indexes...>)
            {
                int dummy[] = {0, ((void) copyFieldAt<Indexes>(ref, message), 0)...};
                (void) dummy;
            }

            template<size_t Index>
            static void copyFieldAt(const Ref& ref, Message& message)
            {
                if (ref.allBits.test(Index))
                {
                    const auto& srcField = meta::get<Index>(ref.values);
                    auto& dstField = meta::get<Index>(message.values);

                    copyField(srcField, dstField);
                    message.allBits.set(Index);
                }
            }

            template<typename FieldRef, typename Field>
            static void copyField(const FieldRef& src, Field& dst)
            {
                FieldRefCast<FieldRef>::cast(src, dst);
            }
        };

    };

    template<typename VersionT, typename Chars, typename... Tags>
    VersionnedMessage<VersionT, Chars, Tags...> fromRef(const VersionnedMessageRef<VersionT, Chars, Tags...>& ref)
    {
        using Ref = VersionnedMessageRef<VersionT, Chars, Tags...>;
        return details::FromRef<Ref>::fromRef(ref);
    }

    // ------------------------------------------------
    // operations
    // ------------------------------------------------

    // get / set operations on a FIX Message


    // identity function used to remove warning C4127 in a constant expression
    // warning C4127 : conditional expression is constant
    template<typename T>
    const T& identity(const T& t)
    {
        return t;
    }

    template<typename Tag, typename Message, typename Value>
    decltype(auto)
    set(Message& message, Value&& value)
    {
        using Index = details::TagIndex<typename Message::TagsList, Tag>;
        using RequiredIndex = details::TagIndex<typename Message::RequiredList, Tag>;

        static_assert(Index::Valid, "Invalid tag for given message");
        static_assert(details::IsValidTypeFor<Tag, Value>::value, "Invalid data type for given Tag");

        meta::get<Index::Value>(message.values).set(std::forward<Value>(value));

        message.allBits.set(Index::Value);

        if (identity(RequiredIndex::Valid))
            message.requiredBits.set(static_cast<size_t>(RequiredIndex::Value));
    }

    template<typename Tag, typename Message>
    decltype(auto)
    get(const Message& message)
    {
        using Index = details::TagIndex<typename Message::TagsList, Tag>;
        static_assert(Index::Valid, "Invalid tag for given message");

        if (!message.allBits.test(static_cast<size_t>(Index::Value)))
            throw std::runtime_error("Bad tag access: tag is not present in message");

        return meta::get<Index::Value>(message.values).get();
    }

    template<typename Tag, typename Message>
    View getView(const Message& message)
    {
        //static_assert(Message::IsRef, "Can only retrieve raw tags on ref-message types"); 

        using Index = details::TagIndex<typename Message::TagsList, Tag>;
        static_assert(Index::Valid, "Invalid tag for given message");

        if (!message.allBits.test(static_cast<size_t>(Index::Value)))
            throw std::runtime_error("Bad tag access: tag is not present in message");

        return meta::get<Index::Value>(message.values).view();
    }

    template<typename Tag, typename Message>
    bool
    tryGet(const Message& message, typename Tag::Type::UnderlyingType& value)
    {
        using Index = details::TagIndex<typename Message::TagsList, Tag>;
        static_assert(Index::Valid, "Invalid tag for given message");

        if (!message.allBits.test(static_cast<size_t>(Index::Value)))
            return false;

        value = meta::get<Index::Value>(message.values).get();
        return true;
    }

    template<typename Tag, typename Message>
    bool
    tryUnsafeGet(const Message& message, typename Tag::Type::UnderlyingType& value)
    {
        using Index = details::TagIndex<typename Message::TagsList, Tag>;
        if (!Index::Valid)
            return false;

        if (!message.allBits.test(static_cast<size_t>(Index::Value)))
            return false;

        value = meta::get<Index::Value>(message.values).get();
        return true;
    }

    template<typename Tag, typename Message>
    bool
    tryGetView(const Message& message, View& view)
    {
        static_assert(Message::IsRef, "Can only retrieve raw tags on ref-message types"); 

        using Index = details::TagIndex<typename Message::TagsList, Tag>;
        static_assert(Index::Valid, "Invalid tag for given message");

        if (!message.allBits.test(Index::Value))
            return false;

        view = meta::get<Index::Value>(message.values).view();
        return true;
    }

    template<typename Tag, typename Message>
    decltype(auto)
    createGroup(Message& message, size_t size)
    {
        using GroupIndex = details::TagIndex<typename Message::TagsList, Tag>;
        using RequiredIndex = details::TagIndex<typename Message::RequiredList, Tag>;

        static_assert(GroupIndex::Valid, "Invalid RepeatingGroup for given message");

        if (identity(RequiredIndex::Valid))
            message.requiredBits.set(static_cast<size_t>(RequiredIndex::Value));

        auto& group = meta::get<GroupIndex::Value>(message.values);
        group.reserve(size);

        return makeGroup(group);
    }

} // namespace Fixpp
