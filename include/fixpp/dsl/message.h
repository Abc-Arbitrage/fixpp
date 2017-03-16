/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX message as a collection of tags
*/

#pragma once

#include <tuple>
#include <unordered_map>
#include <bitset>

#include <fixpp/meta.h>
#include <fixpp/dsl/details/unwrap.h>
#include <fixpp/dsl/details/traits.h>
#include <fixpp/dsl/details/flatten.h>

namespace Fix
{

    // ------------------------------------------------
    // MessageBase
    // ------------------------------------------------

    // Stores the list of all fields inside a std::tuple

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

        Fields values;

        std::bitset<RequiredTags> requiredBits;
        std::bitset<TotalTags> allBits;

        using View = std::pair<const char*, size_t>;
        std::unordered_map<int, View> unparsed;
    };


    // ------------------------------------------------
    // Chars
    // ------------------------------------------------

    // A parameter pack of char...

    template<char ...> struct Chars { };

    // ------------------------------------------------
    // MessageRef
    // ------------------------------------------------

    // A "view" on a Message
    //
    
    template<typename MsgType, typename... Tags> struct MessageRef;

    template<char... MsgTypeChar, typename... Tags>
    struct MessageRef<Chars<MsgTypeChar...>, Tags...> : public MessageBase<FieldRef, Tags...>
    {
        static constexpr const char MsgType[] = { MsgTypeChar... };
        static constexpr size_t MsgTypeLen = sizeof...(MsgTypeChar);

        using MsgTypeChars = Chars<MsgTypeChar...>;
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
        static constexpr const char MsgType[] = { MsgTypeChar... };
        static constexpr size_t MsgTypeLen = sizeof...(MsgTypeChar);

        using MsgTypeChars = Chars<MsgTypeChar...>;
    };

    template<char... MsgTypeChar, typename... Tags>
    constexpr const char MessageT<Chars<MsgTypeChar...>, Tags...>::MsgType[];

    template<char... MsgTypeChar, typename... Tags>
    constexpr const char MessageRef<Chars<MsgTypeChar...>, Tags...>::MsgType[];

    // ------------------------------------------------
    // VersionnedMessage
    // ------------------------------------------------

    // A Message that knows its FIX version

    template<typename VersionT, typename Chars, typename... Tags>
    struct VersionnedMessage : public MessageT<Chars, Tags...>
    {
        using Version = VersionT;
    };

    // ------------------------------------------------
    // operations
    // ------------------------------------------------
 
    // get / set operations on a FIX Message

    template<typename Tag, typename Message, typename Value>
    typename std::enable_if<details::IsValidTag<Message, Tag>::value, void>::type
    set(Message& message, Value&& value)
    {
        static_assert(details::IsValidTypeFor<Tag, Value>::value,
                      "Invalid data type for given Tag");

        static constexpr int TagIndex = meta::typelist::ops::IndexOf<typename Message::TagsList, Tag>::value;
        static constexpr int RequiredBit = meta::typelist::ops::IndexOf<typename Message::RequiredList, Tag>::value;

        std::get<TagIndex>(message.values).set(std::forward<Value>(value));

        message.allBits.set(TagIndex);

        if (RequiredBit != -1)
            message.requiredBits.set(RequiredBit);
    }

    template<typename Tag, typename Message>
    typename std::enable_if<
                details::IsValidTag<Message, Tag>::value, typename Tag::Type::UnderlyingType
             >::type
    get(const Message& message)
    {
        static constexpr size_t Index = meta::typelist::ops::IndexOf<typename Message::TagsList, Tag>::value;
        return std::get<Index>(message.values).get();
    }

    template<typename Tag, typename Message>
    typename std::enable_if<
                details::IsValidGroup<Message, Tag>::value,
                std::vector<typename details::GroupTraits<Message, Tag>::Ref>
            >::type
    get(const Message& message)
    {
        using GroupT = typename details::GroupTraits<Message, Tag>::Type;

        static constexpr int Index = meta::typelist::ops::IndexOf<typename Message::TagsList, GroupT>::value;
        return std::get<Index>(message.values).get();
    }

    template<typename Tag, typename Message>
    typename std::enable_if<
                details::IsValidTag<Message, Tag>::value, bool
             >::type
    tryGet(const Message& message, typename Tag::Type::UnderlyingType& value)
    {
        static constexpr size_t Index = meta::typelist::ops::IndexOf<typename Message::TagsList, Tag>::value;
        if (!message.allBits.test(Index))
            return false;

        value = std::get<Index>(message.values).get();
        return true;
    }

    template<typename Tag, typename Message>
    typename std::enable_if<
        details::IsValidGroup<Message, Tag>::value,
        Group<typename details::GroupTraits<Message, Tag>::Type>
    >::type
    createGroup(Message& message, size_t size)
    {
        using GroupT = typename details::GroupTraits<Message, Tag>::Type;

        static constexpr int Index = meta::typelist::ops::IndexOf<typename Message::TagsList, GroupT>::value;
        static constexpr int RequiredBit = meta::typelist::ops::IndexOf<typename Message::RequiredList, GroupT>::value;

        if (RequiredBit != -1)
            message.requiredBits.set(RequiredBit);

        return Group<GroupT> (std::get<Index>(message.values));
    }

} // namespace Fix
