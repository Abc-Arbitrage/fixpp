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
		std::unordered_map<int, View> unparsed;
	};

	// ------------------------------------------------
	// Chars
	// ------------------------------------------------

	// A parameter pack of char...

	template<char ...> struct Chars { };

    using Empty = Chars<>;

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
		static constexpr const char MsgType[] = { MsgTypeChar... };
		static constexpr size_t MsgTypeLen = sizeof...(MsgTypeChar);

		using MsgTypeChars = Chars<MsgTypeChar...>;
	};

	template<char... MsgTypeChar, typename... Tags>
	constexpr const char MessageT<Chars<MsgTypeChar...>, Tags...>::MsgType[];

	template<char... MsgTypeChar, typename... Tags>
	constexpr const char MessageRef<Chars<MsgTypeChar...>, Tags...>::MsgType[];

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
                    copyInstanceFields(src, dst, meta::make_index_sequence<FieldRef::TotalTags>());
                }

                template<typename FieldRef, typename Field, size_t ... Indexes>
                static void copyInstanceFields(const FieldRef& src, Field& dst, meta::index_sequence<Indexes...>)
                {
                    int dummy[] = {0, ((void) copyInstanceFieldAt<Indexes>(src, dst), 0)...};
                    (void) dummy;
                }

                template<size_t Index, typename FieldRef, typename Field>
                static void copyInstanceFieldAt(const FieldRef& src, Field& dst)
                {
                    if (src.allBits.test(Index))
                    {
                        const auto& srcField = std::get<Index>(src.values);
                        auto& dstField = std::get<Index>(dst.values);
                        copyInstanceField(srcField, dstField);
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
                copyFields(ref, message, meta::make_index_sequence<Ref::TotalTags>());
                return message;
            }

            template<size_t... Indexes>
            static void copyFields(const Ref& ref, Message& message, meta::index_sequence<Indexes...>)
            {
                int dummy[] = {0, ((void) copyFieldAt<Indexes>(ref, message), 0)...};
                (void) dummy;
            }

            template<size_t Index>
            static void copyFieldAt(const Ref& ref, Message& message)
            {
                if (ref.allBits.test(Index))
                {
                    const auto& srcField = std::get<Index>(ref.values);
                    auto& dstField = std::get<Index>(message.values);

                    copyField(srcField, dstField);
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
    typename std::enable_if<details::IsValidTag<Message, Tag>::value, void>::type
    set(Message& message, Value&& value)
    {
        static_assert(details::IsValidTypeFor<Tag, Value>::value,
                      "Invalid data type for given Tag");

        static constexpr int TagIndex = meta::typelist::ops::IndexOf<typename Message::TagsList, Tag>::value;
        static constexpr int RequiredBit = meta::typelist::ops::IndexOf<typename Message::RequiredList, Tag>::value;

        std::get<TagIndex>(message.values).set(std::forward<Value>(value));

        message.allBits.set(TagIndex);

        if (identity(RequiredBit) != -1)
            message.requiredBits.set(static_cast<size_t>(RequiredBit));
    }

    template<typename Tag, typename Message>
    decltype(auto)
    get(const Message& message, typename std::enable_if<details::IsValidTag<Message, Tag>::value, void>::type * = nullptr)
     {
        static constexpr size_t Index = meta::typelist::ops::IndexOf<typename Message::TagsList, Tag>::value;
        return std::get<Index>(message.values).get();
    }

    template<typename Tag, typename Message>
    decltype(auto)
    get(const Message& message, typename std::enable_if<details::IsValidGroup<Message, Tag>::value, void>::type * = nullptr)
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

        if (identity(RequiredBit) != -1)
            message.requiredBits.set(static_cast<size_t>(RequiredBit));

		auto& group = std::get<Index>(message.values);
		group.reserve(size);
        return Group<GroupT> (group);
    }

} // namespace Fix
