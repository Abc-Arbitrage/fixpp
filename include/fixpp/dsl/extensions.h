/* message.h
   Mathieu Stefani, 04 january 2017
   
  A collection of utilities to extend a message
*/

#pragma once

namespace Fix
{

    template<typename MessageT, typename... ExtensionTags> struct ExtendedMessage;

    template<typename VersionT, char MsgTypeChar, typename... Tags, typename... ExtensionTags>
    struct ExtendedMessage<
             VersionnedMessage<VersionT, MsgTypeChar, Tags...>, ExtensionTags...
           > : public VersionnedMessage<VersionT, MsgTypeChar, Tags..., ExtensionTags...>
    {
    };

    namespace impl
    {
        template<typename Tag, typename Type>
        struct ChangeType { };

        template<unsigned N, typename T, typename NewType>
        struct ChangeType<TagT<N, T>, NewType>
        {
            using Type = TagT<N, NewType>;
        };

        template<typename Tag>
        struct AddTag { };

        template<typename GroupTag, typename... Tags>
        struct ExtendGroup { };

        template<typename T, typename Change>
        struct ApplyOne;

        template<typename VersionT, char MsgTypeChar, typename... Tags, typename Tag, typename Type>
        struct ApplyOne<
            VersionnedMessage<VersionT, MsgTypeChar, Tags...>,
            ChangeType<Tag, Type>
        >
        {
            template<typename SrcTag, typename DstTag> struct Impl
            {
                using Result = SrcTag;
            };

            template<unsigned N, typename T> struct Impl<TagT<N, T>, Tag>
            {
                using Result = TagT<N, Type>;
            };

            template<unsigned N, typename T> struct Impl<Required<TagT<N, T>>, Tag>
            {
                using Result = Required<TagT<N, Type>>;
            };

            using Result = VersionnedMessage<VersionT, MsgTypeChar, typename Impl<Tags, Tag>::Result...>;
        };

        template<typename VersionT, char MsgTypeChar, typename... Tags, typename Tag>
        struct ApplyOne<
            VersionnedMessage<VersionT, MsgTypeChar, Tags...>,
            AddTag<Tag>
        >
        {
            using Result = VersionnedMessage<VersionT, MsgTypeChar, Tags..., Tag>;
        };

        template<typename VersionT, char MsgTypeChar, typename... Tags, typename GroupTag, typename... NewTags>
        struct ApplyOne<
            VersionnedMessage<VersionT, MsgTypeChar, Tags...>,
            ExtendGroup<GroupTag, NewTags...>
        >
        {
            template<typename Tag>
            struct Impl
            {
                using Result = Tag;
            };

            template<typename... GroupTags>
            struct Impl<RepeatingGroup<GroupTag, GroupTags...>>
            {
                using Result = RepeatingGroup<GroupTag, GroupTags..., NewTags...>;
            };

            template<typename... GroupTags>
            struct Impl<Required<RepeatingGroup<GroupTag, GroupTags...>>>
            {
                using Result = Required<RepeatingGroup<GroupTag, GroupTags..., NewTags...>>;
            };

            using Result = VersionnedMessage<VersionT, MsgTypeChar, typename Impl<Tags>::Result...>;
        };

        template<typename MessageT, typename... Changes>
        struct ApplyAll;

        template<typename MessageT, typename Change, typename... Rest>
        struct ApplyAll<MessageT,  Change, Rest...>
        {
            using Result = typename ApplyAll<typename ApplyOne<MessageT, Change>::Result, Rest...>::Result;
        };

        template<typename MessageT>
        struct ApplyAll<MessageT>
        {
            using Result = MessageT;
        };

        template<typename MessageT, typename... Changes>
        struct ChangeSet
        {
            using Apply = typename ApplyAll<MessageT, Changes...>::Result;
        };

    };

    template<typename MessageT>
    struct MessageOverwrite
    {
        template<typename... Args> using ChangeSet = impl::ChangeSet<MessageT, Args...>;
        template<typename Tag, typename Type> using ChangeType = impl::ChangeType<Tag, Type>;

        template<typename Tag> using AddTag = impl::AddTag<Tag>;

        template<typename Tag> using Of = Tag;
        template<typename Type> using To = Type;

        template<typename GroupTag, typename... Tags> using ExtendGroup = impl::ExtendGroup<GroupTag, Tags...>;

    };


} // namespace Fix
