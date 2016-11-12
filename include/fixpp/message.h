/* message.h
   Mathieu Stefani, 12 november 2016
   
   A representation of a FIX Message as a typed collection of tags
*/

#pragma once

#include <tuple>
#include <bitset>
#include <type_traits>

#include <fixpp/tag.h>
#include <fixpp/meta.h>

namespace Fix {

template<typename GroupTag, typename... Tags>
struct RepeatingGroup
{
    using Type = GroupTag;
    static constexpr bool Required = GroupTag::Required;
};

namespace details
{

    template<
        typename Message,
        typename Tag,
        int Index = meta::typelist::ops::IndexOf<typename Message::List, Tag>::value
    > struct IsValidTag : public std::true_type { };

    template<typename Tag, typename Message>
    struct IsValidTag<Tag, Message, -1> : public std::false_type { };

    template<
        typename Tag,
        typename Value,
        typename CleanV = typename std::decay<Value>::type,
        bool Valid = std::is_convertible<CleanV, typename Tag::Type>::value>
    struct IsValidTypeFor;

    template<
        typename Tag,
        typename Value,
        typename CleanV>
    struct IsValidTypeFor<Tag, Value, CleanV, false> : public std::false_type { };

    template<
        typename Tag,
        typename Value,
        typename CleanV>
    struct IsValidTypeFor<Tag, Value, CleanV, true> : public std::true_type { };

    template<typename Message, typename Tag>
    struct FilterGroup
    {
        template<typename TagT>
        struct IsGroupOf
        {
            static constexpr bool value = false;
        };

        template<typename GroupTag, typename... Tags>
        struct IsGroupOf<RepeatingGroup<GroupTag, Tags...>>
        {
            static constexpr bool value =
                std::is_same<GroupTag, Tag>::value;
        };

        using List = typename meta::typelist::ops::Filter<
                        typename Message::List,
                        IsGroupOf
                     >::Result;

        static constexpr size_t Length = meta::typelist::ops::Length<List>::value;
        static_assert(Length == 1, "Found more than one matching RepeatingGroup for Message");

        using Type = typename meta::typelist::ops::At<0, List>::Result;
    };

    template<typename Message, typename Tag,
             int Size = FilterGroup<Message, Tag>::Length>
    struct IsValidGroup : public std::true_type { };

    template<typename Message, typename Tag>
    struct IsValidGroup<Message, Tag, 0> : public std::false_type { };

    template<typename Message, typename Tag>
    struct GroupTraits
    {
        using Type = typename FilterGroup<Message, Tag>::Type;
        static constexpr size_t Length = FilterGroup<Message, Tag>::Length;
    };

    template<typename Tag> struct IsRequired : public std::integral_constant<bool, TagTraits<Tag>::Required> { };

} // namespace details


    template<typename TagT>
    struct Field
    {
        using Tag = TagT;
        using Type = typename TagT::Type::UnderlyingType;

        Field()
            : empty_(true)
        { }

        Field(const Field& other)
        {
            if (!other.empty())
            {
                val_ = other.val_;
            }
            empty_ = other.empty_;
        }

        Field(Field&& other)
        {
            if (!other.empty())
            {
                val_ = std::move(other.val_);
                empty_ = false;
                other.empty_ = true;
            }
            else
            {
                empty_ = true;
            }
        }

        template<typename T>
        void set(T&& value)
        {
            val_ = std::forward<T>(value);
        }

        const Type& get() const
        {
            return val_;
        }

        bool empty() const
        {
            return empty_;
        }

    private:        
        Type val_;
        bool empty_;
    };

    template<typename GroupTag, typename... Tags>
    struct Field<RepeatingGroup<GroupTag, Tags...>>
    {
        using Tag = GroupTag;
        using GroupValues = std::tuple<Field<Tags>...>;
        using Type = std::vector<GroupValues>;

        Field() = default;
        Field(const Field& other) = default;
        Field(Field&& other) = default;

        const Type& get() const
        {
            return val_;
        }

        Type& get()
        {
            return val_;
        }

        size_t size() const
        {
            return val_.size();
        }

        void reserve(size_t size)
        {
            val_.reserve(size);
        }

        void push_back(const GroupValues& values)
        {
            val_.push_back(values);
        }

        void push_back(GroupValues&& values)
        {
            val_.push_back(std::move(values));
        }

    private:
        Type val_;
    };


    template<typename TagT>
    struct FieldRef
    {
        using Tag = TagT;
        using Type = typename TagT::Type::UnderlyingType;

        FieldRef() = default;

    private:
        const char* offset;
        size_t size;
    };

    template<template<typename> class FieldT, typename... Tags> struct MessageBase
    {
        using Values = std::tuple<FieldT<Tags>...>;
        using List = typename meta::typelist::make<Tags...>::Result;

        using RequiredList = typename meta::typelist::ops::Filter<List, details::IsRequired>::Result;

        static constexpr size_t RequiredTags = meta::typelist::ops::Length<RequiredList>::value;
        static constexpr size_t TotalTags = sizeof...(Tags);

        Values values;

        std::bitset<RequiredTags> bits;
    };

    template<char MsgTypeChar, typename... Tags> struct MessageT : public MessageBase<Field, Tags...>
    {
        static constexpr const char MsgType = MsgTypeChar;
    };

    template<typename MessageT> struct MessageRef;

    template<char MsgTypeChar, typename... Tags>
    struct MessageRef<MessageT<MsgTypeChar, Tags...>> : public MessageBase<FieldRef, Tags...>
    {
        static constexpr const char MsgType = MsgTypeChar; 
    };

    template<typename RepeatingGroup> struct Group;

    template<typename GroupTag, typename... Tags>
    struct Group<RepeatingGroup<GroupTag, Tags...>>
    {
        using Instance = MessageBase<Field, Tags...>;
        using FieldType = Field<RepeatingGroup<GroupTag, Tags...>>;

        Group(FieldType& field)
            : field(field)
        {
        }

        Instance instance() const
        {
            return Instance { };
        }

        void add(const Instance& instance)
        {
            field.push_back(instance.values);
        }

        void add(Instance&& instance)
        {
            field.push_back(std::move(instance.values));
        }

        size_t size() const
        {
            return field.size();
        }

    private:
        FieldType& field;
    };

    template<char MsgTypeChar, typename... Tags>
    constexpr const char MessageT<MsgTypeChar, Tags...>::MsgType;

    template<char MsgTypeChar, typename... Tags>
    constexpr const char MessageRef<MessageT<MsgTypeChar, Tags...>>::MsgType;

    template<typename Tag, typename Message, typename Value>
    typename std::enable_if<details::IsValidTag<Message, Tag>::value, void>::type
    set(Message& message, Value&& value)
    {
        static_assert(details::IsValidTypeFor<Tag, Value>::value,
                      "Invalid data type for given Tag");

        static constexpr int Index = meta::typelist::ops::IndexOf<typename Message::List, Tag>::value;
        static constexpr int Bit = meta::typelist::ops::IndexOf<typename Message::RequiredList, Tag>::value;

        std::get<Index>(message.values).set(std::forward<Value>(value));

        if (TagTraits<Tag>::Required)
            message.bits.set(Bit);
    }

    template<typename Tag, typename Message>
    typename std::enable_if<details::IsValidTag<Message, Tag>::value, typename Tag::Type::UnderlyingType>::type
    get(const Message& message)
    {
        static constexpr size_t Index = meta::typelist::ops::IndexOf<typename Message::List, Tag>::value;
        return std::get<Index>(message.values).get();
    }

    template<typename Tag, typename Message>
    typename std::enable_if<
        details::IsValidGroup<Message, Tag>::value,
        Group<typename details::GroupTraits<Message, Tag>::Type>
    >::type
    createGroup(Message& message, size_t size)
    {
        using GroupT = typename details::GroupTraits<Message, Tag>::Type;

        static constexpr int Index = meta::typelist::ops::IndexOf<typename Message::List, GroupT>::value;
        return Group<GroupT> (std::get<Index>(message.values));
    }

} // namespace Fix
