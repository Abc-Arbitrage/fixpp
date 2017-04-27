/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX Repeating Group
*/

#pragma once

#include <vector>
#include <sstream>

#include <fixpp/dsl/field.h>
#include <fixpp/utils/SmallVector.h>

namespace Fix
{

    template<template<typename> class FieldT, typename... Tags> struct MessageBase;

    template<size_t Size> struct SizeHint { };

    // ------------------------------------------------
    // RepeatingGroup
    // ------------------------------------------------

    // A FIX repeating group

    template<typename GroupTag, typename SizeHint, typename... Tags>
    struct SmallRepeatingGroup
    {
        using Type = GroupTag;
    };


    // ------------------------------------------------
    // Field
    // ------------------------------------------------

    // Specialization of a Field for a repeating group.
    // Stores values inside a SmallVector

	template<typename Group> struct InstanceGroup;

	template<typename GroupTag, typename SizeHint, typename... Tags>
	struct InstanceGroup<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>> : public MessageBase<Field, Tags...>
	{
	};

    template<typename GroupTag, size_t Size, typename... Tags>
    struct Field<SmallRepeatingGroup<GroupTag, SizeHint<Size>, Tags...>>
    {
        using Tag = GroupTag;
		using GroupType = InstanceGroup<SmallRepeatingGroup<GroupTag, SizeHint<Size>, Tags...>>;

        using Type = llvm::SmallVector<GroupType, Size>;

        static constexpr size_t TotalTags = GroupType::TotalTags;

        Field() = default;
        Field(const Field& other) = default;
        Field& operator=(const Field& other) = default;

        Field(Field&& other) = default;
        Field& operator=(Field&& other) = default;

        constexpr unsigned tag() const
        {
            return Tag::Id;
        }

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

        void push_back(const GroupType& values)
        {
            val_.push_back(values);
        }

        void push_back(GroupType&& values)
        {
            val_.push_back(std::move(values));
        }

        bool empty() const
        {
            return val_.empty();
        }

    private:
        Type val_;
    };

    // ------------------------------------------------
    // FieldRef
    // ------------------------------------------------

    // Specialization of a FieldRef for a repeating group.
    // Represents a "view" on a repeating group

	template<typename Group> struct InstanceGroupRef;

	template<typename GroupTag, typename SizeHint, typename... Tags>
	struct InstanceGroupRef<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>> : public MessageBase<FieldRef, Tags...>
	{
	};

    template<typename GroupTag, size_t Size, typename... Tags>
    struct FieldRef<SmallRepeatingGroup<GroupTag, SizeHint<Size>, Tags...>>
    {
        using Tag = GroupTag;

		using GroupType = InstanceGroupRef<SmallRepeatingGroup<GroupTag, SizeHint<Size>, Tags...>>;
        using Values = llvm::SmallVector<GroupType, Size>;

        constexpr unsigned tag() const
        {
            return GroupTag::Id;
        }

        template<typename TypeRef>
        void add(TypeRef&& ref)
        {
            values.push_back(std::forward<TypeRef>(ref));
        }

        void reserve(size_t size)
        {
            values.reserve(size);
        }

        Values get() const
        {
            return values;
        }

        size_t size() const
        {
            return values.size();
        }

    private:
        Values values;
    };

    // ------------------------------------------------
    // Group
    // ------------------------------------------------

    template<typename RepeatingGroup> struct Group;

    template<typename GroupTag, typename SizeHint, typename... Tags>
    struct Group<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>
    {
		using Instance = InstanceGroup<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>;
        using FieldType = Field<SmallRepeatingGroup<GroupTag, SizeHint, Tags...>>;

        Group(FieldType& field)
            : field(field)
        {
        }

        Instance instance() const
        {
            return {};
        }

        void add(const Instance& instance)
        {
            checkRequiredFields(instance);
            field.push_back(instance);
        }

        void add(Instance&& instance)
        {
            checkRequiredFields(instance);
            field.push_back(std::move(instance));
        }

        size_t size() const
        {
            return field.size();
        }

    private:
        template<typename InstanceT> void checkRequiredFields(InstanceT&& instance) const
        {
            if (!instance.requiredBits.all())
            {
                std::ostringstream error;
                const size_t missingBits = instance.requiredBits.size() - instance.requiredBits.count();
                error << "Missing " << missingBits << " required value(s) for Instance";
                throw std::runtime_error(error.str());
            }
        }

        FieldType& field;
    };

    template<typename GroupTag, typename... Tags>
    using RepeatingGroup = SmallRepeatingGroup<GroupTag, SizeHint<10>, Tags...>;

} // namespace Fix
