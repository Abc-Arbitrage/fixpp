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

        template<typename List, typename Tag>
        struct TagIndex
        {
            template<typename TagT>
            struct IsTag
            {
                static constexpr bool value =
                    std::is_same<TagT, Tag>::value;
            };

            template<typename GroupTag, typename SizeHint, typename... Tags>
            struct IsTag<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>
            {
                static constexpr bool value =
                    std::is_same<GroupTag, Tag>::value;
            };

            static constexpr int Value = meta::typelist::ops::Find<List, IsTag>::Result;
            static constexpr bool Valid = (Value != -1);
        };

        template<typename List, typename Tag>
        constexpr int TagIndex<List, Tag>::Value;

        template<typename List, typename Tag>
        constexpr bool TagIndex<List, Tag>::Valid;

        template<typename Tag> struct IsRequired : std::false_type { };
        template<typename Tag> struct IsRequired<Required<Tag>> : std::true_type { };

    } // namespace deatails

} // namespace Fixpp
