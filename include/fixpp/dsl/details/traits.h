#pragma once

namespace Fixpp
{
    template<typename GroupTag, typename SizeHint, typename...> struct SmallRepeatingGroup;
    template<typename Group> struct GroupRef;
    template<typename T> struct Required;

    namespace details
    {
        template<
            typename Message,
            typename Tag,
            int Index = meta::typelist::ops::IndexOf<typename Message::TagsList, Tag>::value
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

            template<typename GroupTag, typename SizeHint, typename... Tags>
            struct IsGroupOf<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>
            {
                static constexpr bool value =
                    std::is_same<GroupTag, Tag>::value;
            };

            using List = typename meta::typelist::ops::Filter<
                            typename Message::TagsList,
                            IsGroupOf
                         >::Result;

            static constexpr size_t Length = meta::typelist::ops::Length<List>::value;
            static_assert(Length <= 1, "Found more than one matching RepeatingGroup for Message");

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

        template<typename Tag> struct IsRequired : std::false_type { };
        template<typename Tag> struct IsRequired<Required<Tag>> : std::true_type { };

    } // namespace deatails

} // namespace Fixpp
